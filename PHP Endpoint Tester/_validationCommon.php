<?php

function validateEndpoint($endpoint, $responseName, &$endpointParameters, $validatorCallback)
{
    $paramKeys = array_keys($endpointParameters);

    //Fillout the URL parameters
    foreach ($paramKeys as $key)
    {
        if (strpos($endpoint, "[$key]") !== false)
        {
            $endpoint = str_replace("[$key]", $endpointParameters[$key], $endpoint);
        }
    }
;
    $return =& $endpointParameters['return'];
    if (!isset($return->responses))
    {
        $return->responses = new stdClass();
    }

    $retries = getenv('REQUEST_RETRIES');
    for ($i = 0; $i < $retries; ++$i)
    {
        echo "Running $endpoint\n";

        $request = StaticFunctions::MakeNetRequest($endpoint);
        $requestDataRaw = $request->get();
        $responseData = json_decode($requestDataRaw);

        $return->http_code = $request->getResultCode();
        $return->status = $responseData->status;
        $return->warn = $i > 0 || ($return->responses->warn ?? false);

        $return->responses->$responseName = new stdClass();
        $return->responses->$responseName->endpoint = $endpoint;
        $return->responses->$responseName->response = $responseData;
        $return->responses->$responseName->retries = $i;

        if ($request->getResultCode() == 0 && $i != $retries -1)//Timeout
        {
            echo "Retrying. Attempt $i";
        }
        elseif ($request->getResultCode() == 200 && $responseData->status == "ok")
        {
            $parameters = array($responseData, &$endpointParameters);
            break;
        }
        else
        {
            $return->reason = "Failed to run $responseName endpoint: $endpoint Result code:{$request->getResultCode()}\nStatus: $responseData->status";
            return false;
        }
    }

    return call_user_func_array($validatorCallback, $parameters);
}

function getRuntimeData($dataDirectory)
{
    return getSavedDocJson($dataDirectory."validation_runtime_data");
}

function getResultData($dataDirectory)
{
    return getSavedDocJson($dataDirectory."validation_result");
}

function saveRuntimeData($data, $dataDirectory)
{
    saveJsonDoc($data, $dataDirectory."validation_runtime_data");
}

function saveResultData($data, $dataDirectory)
{
    saveJsonDoc($data, $dataDirectory."validation_result");
}

function deleteRuntimeData($dataDirectory)
{
    deleteJsonDoc("validation_runtime_data");
}

function deleteResultData($dataDirectory)
{
    deleteJsonDoc("validation_result");
}
