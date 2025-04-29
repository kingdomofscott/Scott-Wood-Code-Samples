<?php if (!isset($data['isSuspended']) || !$data['isSuspended']) { ?>
<link rel="stylesheet" type="text/css" href="<?php echo $data['gameServerBaseURL']; ?>public/css/psb_webgl.css?version=<?php echo $data['buildNumber']; ?>" >

    <script src="<?php echo $data['awsServerBaseURL']; ?>WebGL.loader.js?version=<?php echo $data['buildNumber']; ?>"></script>
    <script src="<?php echo $data['awsServerBaseURL']; ?>WebGL.init.js?version=<?php echo $data['buildNumber']; ?>"></script>

    <script src="<?php echo $data['gameServerBaseURL']; ?>public/js/unity/Preloader/ClientPreloader.js?version=<?php echo $data['buildNumber']; ?>"></script>

    <script>
        $(document).ready(function()
        {
            if(typeof clientPreloader !== 'undefined' && clientPreloader)
            {
                preloadData =
                    {
                        serverURLBase: '<?php echo $data['gameServerBaseURL']; ?>',
                        clientVersion: '<?php echo $data['bundleVersion']; ?>'.trim() ,
                        clientUniqueID: '<?php echo $data['uniqueID']; ?>'.trim() ,
                        workerJSPath : "<?php echo $data['gameServerBaseURL']; ?>public/js/unity/Preloader/PreloadWorker.js?version=<?php echo $data['buildNumber']; ?>"
                    };

                if(preloadData.clientVersion.length > 0 && preloadData.clientUniqueID.length > 0)
                {
                    clientPreloader.StartPreload(preloadData);
                }
            }
            onWebGLResize();
            initWebGL('<?php echo $data['awsServerBaseURL']; ?>');
        });
    </script>

    <div id="webglContent" style="margin-left: 0px;">
        <div id="gameContainer" style="padding: 0px; margin: 0px; border: 0px; background: rgb(255, 255, 255); z-index: 10;">
            <canvas id="canvas" style="cursor:default;"></canvas>
        </div>
    </div>

    <div id="splashContainer">
        <div id="splashContent">
            <img src="<?php echo $data['gameServerBaseURL']; ?>public/img/webgl_loading/WebGLLoadingScreen.jpg" alt="Loading Screen">
            <div id="loadingBox">
                <div id="bgBar">
                    <div id="progress_message">0%</div>
                    <div id="bgBackground" style="background-image: url('<?php echo $data['gameServerBaseURL']; ?>public/img/webgl_loading/loadingBarContainer.png')">
                        <div id="bgBackground_mask">
                            <div id="progressBarLeft">
                                <img src="<?php echo $data['gameServerBaseURL']; ?>public/img/webgl_loading/loadingBarEndCap.png" alt="Load Bard End Cap">
                            </div>
                            <div id="progressBar" style="background-image: url('<?php echo $data['gameServerBaseURL']; ?>public/img/webgl_loading/loadingBarFillSlice.png')"></div>
                            <div id="progressBarRight">
                                <img src="<?php echo $data['gameServerBaseURL']; ?>public/img/webgl_loading/loadingBarEndCapRight.png" alt="Load Bard End Cap">
                            </div>
                        </div>
                    </div>
                </div>

                <!-- <p id="loadingInfo">Loading...</p> -->
            </div>
        </div>
    </div>

    <div id="messageContainer">
        <div id="loadingWrapper"></div>
    </div>
<?php } ?>
