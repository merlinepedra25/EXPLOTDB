# The Exploit Database Git Repository
This is an official repository of [The Exploit Database](http://www.exploit-db.com/), a [project](https://www.offensive-security.com/community-projects/) sponsored by [Offensive Security](https://www.offensive-security.com/).
Our repositories are:

  - Exploits & Shellcodes: [https://github.com/offensive-security/exploit-database](https://github.com/offensive-security/exploit-database)
  - Binary Exploits: [https://github.com/offensive-security/exploit-database-bin-sploits](https://github.com/offensive-security/exploit-database-bin-sploits)
  - Papers: [https://github.com/offensive-security/exploit-database-papers](https://github.com/offensive-security/exploit-database-papers)

The Exploit Database is an archive of public exploits and corresponding vulnerable software, developed for use by penetration testers and vulnerability researchers. Its aim is to serve as the most comprehensive collection of [exploits](https://www.exploit-db.com/browse/), [shellcode](https://www.exploit-db.com/shellcode/) and [papers](https://www.exploit-db.com/papers/) gathered through direct submissions, mailing lists, and other public sources, and present them in a freely-available and easy-to-navigate database. The Exploit Database is a repository for exploits and Proof-of-Concepts rather than advisories, making it a valuable resource for those who need actionable data right away.

This repository is updated daily with the most recently added submissions. Any additional resources can be found in our [binary sploits repository](https://github.com/offensive-security/exploit-database-bin-sploits).

Exploits are located in the `/exploit/` directory, shellcodes can be found in the `/shellcode/` directory and `/files_papers.csv` is an [index file](https://github.com/offensive-security/exploit-database/blob/master/files.csv) for the whole archive.

Included with this repository is the **SearchSploit** utility, which will allow you to search through exploits and shellcodes using one or more terms.
For more information, please see the [SearchSploit manual](https://www.exploit-db.com/searchsploit/).

```
root@kali:~# searchsploit -h
  Usage: searchsploit [options] term1 [term2] ... [termN]

==========
 Examples
==========
  searchsploit afd windows local
  searchsploit -t oracle windows
  searchsploit -p 39446
  searchsploit linux kernel 3.2 --exclude="(PoC)|/dos/"

  For more examples, see the manual: https://www.exploit-db.com/searchsploit/

=========
 Options
=========
   -c, --case     [Term]      Perform a case-sensitive search (Default is inSEnsITiVe).
   -e, --exact    [Term]      Perform an EXACT match on exploit title (Default is AND) [Implies "-t"].
   -h, --help                 Show this help screen.
   -j, --json     [Term]      Show result in JSON format.
   -m, --mirror   [EDB-ID]    Mirror (aka copies) an exploit to the current working directory.
   -o, --overflow [Term]      Exploit titles are allowed to overflow their columns.
   -p, --path     [EDB-ID]    Show the full path to an exploit (and also copies the path to the clipboard if possible).
   -t, --title    [Term]      Search JUST the exploit title (Default is title AND the file's path).
   -u, --update               Check for and install any exploitdb package updates (deb or git).
   -w, --www      [Term]      Show URLs to Exploit-DB.com rather than the local path.
   -x, --examine  [EDB-ID]    Examine (aka opens) the exploit using $PAGER.
       --colour               Disable colour highlighting in search results.
       --id                   Display the EDB-ID value rather than local path.
       --nmap     [file.xml]  Checks all results in Nmap's XML output with service version (e.g.: nmap -sV -oX file.xml).
                                Use "-v" (verbose) to try even more combinations
       --exclude="term"       Remove values from results. By using "|" to separated you can chain multiple values.
                                e.g. --exclude="term1|term2|term3".

=======
 Notes
=======
 * You can use any number of search terms.
 * Search terms are not case-sensitive (by default), and ordering is irrelevant.
   * Use '-c' if you wish to reduce results by case-sensitive searching.
   * And/Or '-e' if you wish to filter results by using an exact match.
 * Use '-t' to exclude the file's path to filter the search results.
   * Remove false positives (especially when searching using numbers - i.e. versions).
 * When updating or displaying help, search terms will be ignored.

root@kali:~#
root@kali:~# searchsploit afd windows local
---------------------------------------------------------------------------------------- -----------------------------------
 Exploit Title                                                                          |  Path
                                                                                        | (/usr/share/exploitdb/)
---------------------------------------------------------------------------------------- -----------------------------------
Microsoft Windows (x86) - 'afd.sys' Local Privilege Escalation (MS11-046)               | exploits/win_x86/local/40564.c
Microsoft Windows - 'AfdJoinLeaf' Local Privilege Escalation (MS11-080) (Metasploit)    | exploits/windows/local/21844.rb
Microsoft Windows - 'afd.sys' Local Kernel (PoC) (MS11-046)                             | exploits/windows/dos/18755.c
Microsoft Windows 7 (x64) - 'afd.sys' Dangling Pointer Privilege Escalation (MS14-040)  | exploits/win_x86-64/local/39525.py
Microsoft Windows 7 (x86) - 'afd.sys' Dangling Pointer Privilege Escalation (MS14-040)  | exploits/win_x86/local/39446.py
Microsoft Windows XP - 'afd.sys' Local Kernel Denial of Service                         | exploits/windows/dos/17133.c
Microsoft Windows XP/2003 - 'afd.sys' Local Privilege Escalation (K-plugin) (MS08-066)  | exploits/windows/local/6757.txt
Microsoft Windows XP/2003 - 'afd.sys' Local Privilege Escalation (MS11-080)             | exploits/windows/local/18176.py
---------------------------------------------------------------------------------------- -----------------------------------
root@kali:~#
root@kali:~# searchsploit -p 39446
Exploit: Microsoft Windows 7 (x86) - 'afd.sys' Dangling Pointer Privilege Escalation (MS14-040)
    URL: https://www.exploit-db.com/exploits/39446/
   Path: /usr/share/exploitdb/exploits/win_x86/local/39446.py

Copied EDB-ID 39446's path to the clipboard.

root@kali:~#
```

SearchSploit requires either "CoreUtils" or "utilities" (e.g. `bash`, `sed`, `grep`, `awk`, etc.) for the core features to work. The self updating function will require `git`, and the Nmap XML option to work, will require `xmllint` (found in the `libxml2-utils` package in Debian-based systems).
