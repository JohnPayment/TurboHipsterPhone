<?php

/*
THE PACKAGE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS'
AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES. THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT
ARE DISCLAIMED TO THE EXTENT PERMITTED BY YOUR LOCAL LAW. UNLESS REQUIRED
BY LAW, NO COPYRIGHT HOLDER OR CONTRIBUTOR WILL BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING IN ANY WAY OUT OF
THE USE OF THE PACKAGE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/************************************************************************/
/************ Prerequisites *********************************************/
/************************************************************************/

/*
This library uses LW_encrypt and LW_decrypt which is in the PHP extension
that you should have got along with this distribution. Install that
extension before using this library.
*/

/************************************************************************/
/************ Interfaces Provided By This Library ***********************/
/************************************************************************/

/*

ProcessHTTPMessage()
This can be directly called from index.php. This will process the encrypted XML content in the message, process them and send encrypted XML response.

ProcessTextMessage($msg)
This has to be called by passing the encrypted text content. This will process it and send the encrypted text reponse through SMS.

SendSMSRequest($action, $request, $params) 
This is used to originate a dialog by sending an encrypted text request by SMS. See the implementaion for details.

*/

/************************************************************************/
/************ Interfaces To Be Implemented By The Integrator ************/
/************************************************************************/

/*
function GetNewCtag() 
{
    // Ctag should be maintained as a counter on the DB
    $ctag = "34456";
    return $ctag;
}

//Gives list of param name and its value for a user, when phone is requested(GetConfig) or when server sends 
//change request. 
function GetUserConfigForDevice($user_id, $params)
{
    // Config params for users should be maintained on the DB
    $values = array(); //store the result here
    // Example:
    $values[] = array('name' => "EncryptionKey", 'value' => "disabled"); 

    return $values;
}

//Required for sending SMS response
function GetMSISDN($user_id)
{
    // MSISDN for users should be maintained on the DB
    $msisdn = ""; 

    return $msisdn;
}

// Send 8-bit binary OTA SMS to the port specified
function SendSMS($msisdn, $port, $msg)
{
}

// This is the key for encryption for this system
function GetEncryptionKey() 
{
    return "disabled";
}

// This is the processing that this system wants to do
function StoreIncomingMessage(&$vals, $msg) 
{
    debug_log("StoreIncomingMessage:\n".$msg);
}

// This is the processing that this system wants to do
function StoreOutgoingMessage(&$vals, $msg) 
{
    debug_log("StoreOutgoingMessage:\n".$msg);
}

// This is the processing that this system wants to do
function StoreErrorMessage($msg)
{
    debug_log("StoreErrorMessage:\n".$msg);
}

// Used to log debug messages
function debug_log($message)
{
    list($usec, $sec) = explode(" ", microtime(false));
    $usec = substr($usec, 1, -2);
    $stamp = date("Ymd G:i:s").$usec.'|';
    return error_log($stamp.$message."\n", 3, '/var/sonim/xpas/log/php_debug.log');
}

*/





/************************************************************************/
/*** Lone Worker Server Library *****************************************/
/************************************************************************/

$LW_DISABLED_KEY = 'disabled';

/*** XML and Text Message Parsers ***************************************/

function ParseXML($xml)
{ 
    $vals = array();

    $vals['syntax_error'] = 'Y'; //syntax error

    $result = XMLParser($xml);
    $i = 0;

    if (!isset($result[$i]['tag'])) {
        return $vals;
    }

    $type = strtolower($result[$i]['tag']); 
    $i++;  
    $vals['type'] = $type;  

    if (!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'action' || !isset($result[$i]['value'])) {
        return $vals;
    }

    $vals['action'] = $result[$i]['value'];
    $i++;

    if(!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'ctag' || !isset($result[$i]['value'])) {
        return $vals;
    }

    $vals['ctag'] = $result[$i]['value'];
    $i++;

    if(!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'uid' || !isset($result[$i]['value'])) {
        return $vals;
    }

    $vals['user_id'] = $result[$i]['value']; 
    $i++;

    if (!isValidType($type)) {
        return $vals;
    }

    $action = strtolower($vals['action']);
    if (!isValidAction($action)) {
        return $vals;
    }

    if(!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'imei' || !isset($result[$i]['value'])) {
        return $vals;
    }
    $vals['imei'] = $result[$i]['value'];
    $i++;

    if(!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'imsi' || !isset($result[$i]['value'])) {
        return $vals;
    }
    $vals['imsi'] = $result[$i]['value'];
    $i++;

    if ($type == 'res' || $type == 'rep') {
        if(!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'status' || !isset($result[$i]['value'])) {
            return $vals;
        }
        $vals['status'] = $result[$i]['value'];
        $i++;
    } else {
        $vals['status'] = 0;
    }

    if ($type == 'req') {
        if ($action == 'getconfig') {
            if (!isset($result[$i]['tag']) || strtolower($result[$i]['tag']) != 'params' || !isset($result[$i]['value'])){
                return $vals;
            }
            $vals['params'] = $result[$i]['value'];
        }
    }

    $vals['syntax_error'] = 'N';

    return $vals;  
}

function isValidType($type)
{
    $types = array('req', 'res', 'rep', 'ack');
    foreach($types as $val) {
        if ($type == $val) {
            return true;
        }
    }
    return false;
}

function isValidAction($action)
{
    $actions = array('location', 'alert', 'cellid', 'health', 'getconfig', 'changeconfig', 'generic');
    foreach($actions as $val) {
        if ($action == $val) {
            return true;
        }
    }
    return false;
}

function XMLParser(&$xml)
{
    $p = xml_parser_create();
    xml_parser_set_option($p, XML_OPTION_TARGET_ENCODING, "UTF-8");
    xml_parse_into_struct($p, $xml, $vals, $index);
    xml_parser_free($p);

    return $vals;
}

function ParseMsg($msg) 
{
    $vals = array();
    $vals['syntax_error'] = 'Y';

    $arr = explode('#', $msg);

    $i = 0;
    if (!isset($arr[$i])) {
        return $vals;
    }

    $type = strtolower($arr[$i]);
    $i++;
    $vals['type'] = $type;

    if (!isset($arr[$i])) {
        return $vals;
    }

    $vals['action'] = $arr[$i];
    $i++;

    if (!isset($arr[$i])) {
        return $vals;
    }
    $vals['ctag'] = $arr[$i];
    $i++;

    if (!isset($arr[$i])) {
        return $vals;
    }
    $vals['user_id'] = $arr[$i];
    $i++;

    if (!isValidType($type)) {
        return $vals;
    }

    $action = strtolower($vals['action']);
    if (!isValidAction($action)) {
        return $vals;
    }

    if (!isset($arr[$i])) {
        return $vals;
    }
    $vals['imei'] = $arr[$i];
    $i++;

    if (!isset($arr[$i])) {
        return $vals;
    }
    $vals['imsi'] = $arr[$i];
    $i++;

    if ($type == 'res' || $type == 'rep') {
        if (!isset($arr[$i])) {
            return $vals;
        }

        $vals['status'] = $arr[$i];
        $i++;
    } else {
        $vals['status'] = 0;
    }

    if ($type == 'req') {
        if ($action == 'getconfig') {
            if (!isset($arr[$i])){
                return $vals;
            }
            $vals['params'] = $arr[$i];
        }
    }

    $vals['syntax_error'] = 'N';

    return $vals; 
}

function GetTagValue($msg, $tag_name)
{
    $val = "";
    $p = strpos($msg, "<".$tag_name.">");
    if ($p > -1) {
        $p = $p + strlen($tag_name) + 2;
        $e = strpos($msg, "</".$tag_name.">", $p);
        if ($e > -1) {
            $val = substr($msg, $p, $e - $p);
        }
    }

    return $val;
}

function GetMsgValue($msg, $position)
{
    $val = "";
    $arr = explode("#", $msg);
    if (isset($arr[$position])) {
        $val = $arr[$position];
    }
    return $val;
}

/*** XML and Text Message Formatters ***************************************/

function FormXML($vals, $params)
{ 
    $type = $vals['type'];
    $xml  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    $xml .= "<".$type.">";

    if ($type != 'ack') {
        $xml .= "<action>".$vals['action']."</action>";
    } 

    $xml .= "<ctag>".$vals['ctag']."</ctag>";
    $xml .= "<uid>".$vals['user_id']."</uid>";

    $xml .= "<imei>".$vals['imei']."</imei>";
    $xml .= "<imsi>".$vals['imsi']."</imsi>";

    if ($type == 'res' || $type == 'ack') {
        $xml .= "<status>".$vals['status']."</status>";
    }

    foreach($params as $tag => $val) {
        if (is_array($val)) {  // required when param name, value pairs are there
            foreach($val as $tag1 => $val1) {
                $xml .= "<".$tag1.">".$val1."</".$tag1.">";
            }
        } else {
            $xml .= "<".$tag.">".$val."</".$tag.">";
        }
    }

    $xml .= "<minutes>". ((date('G') * 60) + date('i')) ."</minutes>";
    $xml .= "</".$type.">";

    return $xml;
}

function FormMsg($vals, $params)
{
    $type = $vals['type'];
    $msg = strtoupper($type).'#';
    if ($type != 'ack') {
        $msg .= $vals['action'].'#';
    }

    $msg .= $vals['ctag'].'#'.$vals['user_id'].'#';
    $msg .= $vals['imei'].'#'.$vals['imsi'].'#';

    if ($type == 'res' || $type == 'ack') {
        $msg .= $vals['status'].'#';
    }

    foreach($params as $tag => $val) {
        if (is_array($val)) {  // required when param name, value pairs are there
            foreach($val as $tag1 => $val1) {
                $msg .= $val1.'#';
            }
        } else {
            $msg .= $val.'#';
        }
    }

    $msg .= "". ((date('G') * 60) + date('i')) ."#";
    return $msg;
}

/*** XML and Text Message Processors ***************************************/

function ProcessDeviceMessages($vals, $msg)
{
    if ($vals['syntax_error'] == 'Y') {
        StoreErrorMessage($msg);
        $vals['status'] = 400;
        if (!isset($vals['ctag'])) {
            if ($vals['mode'] == 'http') {
                $vals['ctag'] = GetTagValue($msg, "ctag");
            } else {
                $vals['ctag'] = GetMsgValue($msg, 2);
            }
        }
    } else {
        if ($vals['type'] == 'rep' && $vals['action'] == 'health'){
            if ($vals['mode'] == 'http') {
                $emg = GetTagValue($msg, "emg"); 
            } else {
                $emg = GetMsgValue($msg, 7);
            } 

            if ($emg == "0"){
                $vals['action'] .= '/OK';
            } else if ($emg == "3"){
                $vals['action'] .= '/alarm';
            } else {
                $vals['action'] .= '/warning';
            }
        } else if ($vals['type'] == 'rep' && $vals['action'] == 'changeconfig'){
            if (strpos($msg, "LoneWorkerMode") !== false) {
                $vals['action'] = 'chgconfig/mode';
            } else if (strpos($msg, "PhoneInfo") !== false) {
                $vals['action'] = 'chgconfig/e2e';
            } else if (strpos($msg, "PhoneStatus") !== false) {
                $vals['action'] = 'chgconfig/e2e';
            } else {
                $vals['action'] = 'chgconfig';
            }
        }

        StoreIncomingMessage($vals, $msg);
        $vals['status'] = 200;
        if ($vals['type'] == 'req' && $vals['action'] == 'getconfig'){
            $params = GetUserConfigForDevice($vals['user_id'], $vals['params']);
            if (count($params) == 0) {
                $vals['status'] = 404;
            }
            return SendResponse($vals, $params);
        }
    }

    return SendAcknowledgement($vals);
}

function SendResponse($values, $params) 
{
    $vals =  array('type' => 'res',
            'action' => isset($values['action'])?$values['action']:'',
            'ctag' => isset($values['ctag'])?$values['ctag']:'',
            'user_id' => isset($values['user_id'])?$values['user_id']:'',
            'status' => $values['status'],
            'imei' => $values['imei'],
            'imsi' => $values['imsi'] );

    if ($values['mode'] == 'http') {
        $msg = FormXML($vals, $params);
    } else {
        $msg = FormMsg($vals, $params);
    }

    StoreOutgoingMessage($vals, $msg);

    return $msg;
}

function SendAcknowledgement($values)
{
    $vals = array('type' => 'ack',
            'action' => '',
            'ctag' => isset($values['ctag'])?$values['ctag']:'',
            'user_id' => isset($values['user_id'])?$values['user_id']:'',
            'status' => $values['status'],
            'imei' => $values['imei'],
            'imsi' => $values['imsi'] );

    $params = array();
    if ($values['mode'] == 'http') {
        $msg = FormXML($vals, $params);
    } else {
        $msg = FormMsg($vals, $params);
    }

    StoreOutgoingMessage($vals, $msg);

    return $msg;
}

function SendEncryptedRequest($msisdn, $msg)
{
    global $LW_DISABLED_KEY;

    $key = GetEncryptionKey();
    if (strtolower($key) != $LW_DISABLED_KEY) {
        LW_encrypt($msg, $key);
    }

    $port = 2948;
    $str = "\124\006\001\266\003\013j\000E\306\000\001U\001\2076\000\000\006\003LW_SMS\000\001\207\005\003LW_APP_DATA\000\006\003".$msg."\000\001\001\001";

    SendSMS($msisdn, $port, $str);
}

/************************************************************************/
/*** Lone Worker Server Main Entry Points *******************************/
/************************************************************************/

function ProcessHTTPMessage()
{
    global $LW_DISABLED_KEY;

    $xml = file_get_contents('php://input');
    $key = GetEncryptionKey();
    if (strtolower($key) != $LW_DISABLED_KEY) {
        LW_decrypt($xml, $key); 
    }

    debug_log("Got:\n".$xml);
    $vals = ParseXML($xml);
    $vals['mode'] = 'http';
    $retval = ProcessDeviceMessages($vals, $xml);
    debug_log("Sent:\n".$retval);

    header("Content-Type: application/xml; charset=utf-8");
    header("HTTP/1.0 200 OK");
    if (strtolower($key) != $LW_DISABLED_KEY) {
        LW_encrypt($retval, $key);
    }

    $len = strlen($retval);
    header("Content-Length: ".$len);
    echo $retval;
}

function ProcessTextMessage($msg)
{
    global $LW_DISABLED_KEY;

    if (strpos($msg, 'NIL') === false){
        $key = GetEncryptionKey();
        if (strtolower($key) != $LW_DISABLED_KEY) {
            LW_decrypt($msg, $key);
        }
        debug_log("Got:\n".$msg);
        $vals = ParseMsg($msg);
        StoreIncomingMessage($vals, $msg);
        if (isset($vals['user_id'])) {
            $vals['mode'] = 'sms';
            $msg = ProcessDeviceMessages($vals, $msg);
            debug_log("Sent:\n".$msg);
            $msisdn = GetMSISDN($vals['user_id']);
            if ($msisdn != "") {
                SendEncryptedRequest($msisdn, $msg);
            }
        }
    }
}

function SendSMSRequest($action, $request, $params) 
{
    $ctag = GetNewCtag();

    $vals = array('type' => 'req',
            'action' => $action,
            'ctag' => $ctag,
            'user_id' => $request['user_id'],
            'status' => 0,
            'imei' => '',
            'imsi' => '');


    $msg = FormMsg($vals, $params);

    $result = StoreOutgoingMessage($vals, $msg);
    debug_log("Sent:\n".$msg);

    SendEncryptedRequest($request['msisdn'], $msg);

    return $result;
}
?>
