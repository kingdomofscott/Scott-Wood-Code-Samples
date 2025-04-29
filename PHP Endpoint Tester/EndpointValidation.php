<?php

require_once '_validationCommon.php';

echo "\n*************************EndpointValidation*************************\n";

$validationScriptDir = getenv('VALIDATION_SCRIPT_DIR');

$writeDirectory = $argv[1];
$baseURL = $argv[2];

//Load Couchbase endpoint_validation_config
$validationConfig = EndpointValidationInfo::loadOrException();
$returnStatus = new stdClass();
$returnStatus->result = array();
$returnStatus->status = "ok";

foreach ($validationConfig->player_data as $pID => $playerData)
{
	echo "\nPlayer ID: $pID\n";
    echo "$playerData->description\n";

    putenv("REQUEST_RETRIES=$validationConfig->network_retries");

	$runtimeData = getRuntimeData($writeDirectory);
	$runtimeData->backendURL = $baseURL;
	$runtimeData->pID = $pID;
	$runtimeData->platform = $playerData->platform;
    saveRuntimeData($runtimeData, $writeDirectory);

    $player = loadPlayer($runtimeData->player_id);
    $runtimeData->level = $player->level;
    saveRuntimeData($runtimeData, $writeDirectory);

    foreach ($playerData->tests as $test)
    {
        $script = null;
        switch ($test) {
            case 'lobby':
                $script = 'LoadLobbyValidation';
                break;
        }

        if (!is_null($script))
        {
            $failed = false;
            try
            {
                echo shell_exec("php {$validationScriptDir}{$script}.php $writeDirectory");
                $validationResult = getResultData($writeDirectory);
                if ($validationResult->status != "ok")
                {
                    $failed = true;
                }
                elseif ($validationResult->warn)
                {
                    $returnStatus->status = "warning";
                }
            }
            catch (Exception $e)
            {
                $failed = true;
            }

            if($failed)
            {
                $returnStatus->status = "failed";
                break;
            }
        }
    }
    

    $newEntry = (object)[
        "status"            => $validationResult->status,
        "http_code"         => $validationResult->http_code,
        "reason"            => $validationResult->reason ?? null,
		"description"       => $playerData->description,
        "clone_player_id"   => $runtimeData->player_id,
        "response_data"     => $validationResult->responses,
        ];

    $returnStatus->result[$pID] = $newEntry;

    deleteResultData($writeDirectory);
    deleteRuntimeData($writeDirectory);
}

saveJsonDoc($returnStatus, $writeDirectory."final_validation_result");

echo "*************************End EndpointValidation*************************\n";

