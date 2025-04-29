<?php

require_once '_validationCommon.php';

const LOBBY = "[baseURL]lobby/mobile/[pID]?[platform]&[sessionToken]";

function lobbyValidator($response, &$endpointParameters)
{
    echo "Successfully validated lobby for {$endpointParameters['pID']}!\n";

    $runtimeData =& $endpointParameters['runtime'];

    //More specific player data checking goes here

    return true;
}

echo "\n*************************LoadLobbyValidation*************************\n";

$writeDirectory = $argv[1];
$returnData = getResultData($writeDirectory);
$runtimeData = getRuntimeData($writeDirectory);

$endpointParameters = array(
    'baseURL'           => $runtimeData->backendURL,
    'pID'               => $runtimeData->player_id,
    'platform'          => "platform=$runtimeData->platform",
    'sessionToken'      => "session_token=$runtimeData->session_token",
    'return'            => $returnData,
    'runtime'           => $runtimeData,
);

validateEndpoint(LOBBY, 'lobby', $endpointParameters, "lobbyValidator");

saveResultData($endpointParameters['return'], $writeDirectory);
saveRuntimeData($endpointParameters['runtime'], $writeDirectory);

echo "*************************End LoadLobbyValidation*************************\n";
