<?php
require_once "LWLibrary.php";

function GetEncryptionKey()
{
        return "disabled";
}

function StoreIncomingMessage(&$vals, $msg)
{
        debug_log("StoreIncomingMessage:\n".$msg);
}

function StoreOutgoingMessage(&$vals, $msg)
{
        debug_log("StoreOutgoingMessage:\n".$msg);
}

function StoreErrorMessage($msg)
{
        debug_log("StoreErrorMessage:\n".$msg);
}

function debug_log($message)
{
        list($usec, $sec) = explode(" ", microtime(false));
        $usec = substr($usec, 1, -2);
        $stamp = date("Ymd G:i:s").$usec.'|';
        return error_log($stamp.$message."\n", 3, '/opt/sonim/xpas/server/LWS/gomu.log');
}

function GetUserConfigForDevice($user_id, $params)
{
    // Config params for users should be maintained on the DB
    $values = array(); //store the result here
    // Example:
    $values[] = array('name' => "EncryptionKey", 'value' => "disabled");

    return $values;
}

ProcessHTTPMessage();
?>
