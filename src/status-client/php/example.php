<?

require_once("njproto.php");

function show_status($host, $port, $user, $pass)
{
  $fp = fsockopen($host, $port, $errno,$errstr,30);
  if (!$fp) {  echo "$errstr ($errno)<br>\n"; return; }

  $poo = nj_getmsg($fp);
  if ($poo && $poo['type'] == 0) // this is a challenge/license message
  {
    $res = nj_parseauthmsg($poo['data']);
    if ($res)
    {
      if ($res['license'])
      {
      echo "Server license:<pre>\n" . $res['license'] . "\n\n</pre>";
      }
      echo "Querying auth...<BR>";
      nj_sendauthmsg($fp, $user, $pass, $res['challenge']);

      while (!feof($fp)) 
      {
        $poo=nj_getmsg($fp);
        if (!$poo) break;

        if ($poo['type'] == 1)
        {
          echo "invalid login to server!<BR>";
          break;
        }

        switch ($poo['type'])
        {
          case 192: // chat message
            $ret=nj_parsechatmsg($poo['data']);
            if ($ret && !strcasecmp($ret[0],"TOPIC"))
            {
               if ($ret[2] != "") echo "Topic: '" .  $ret[2] . "'<BR>";
            }
          break;
          case 2: // beat change etc
        $ret=nj_parsebpmbpimsg($poo['data']);
            if ($ret)
            {
              echo "BPM: " . $ret['bpm'] . ", BPI: " . $ret['bpi'] . "<BR>";
            }
          break;
          case 3: 
            $ret = nj_parseuserinfolist($poo['data']);
            if ($ret)
            {
              reset($ret);
              echo count($ret) . " users:<BR>\n";
              while ($f = key($ret))
              {
                $p=$ret[$f];
                $cnt=$p['cnt'];
                echo "&nbsp;&nbsp;$f ($cnt channels)<BR>\n";
                if (0) for ($x = 0; $x < $cnt; $x ++)
                {
                  echo "&nbsp;&nbsp;channel " . ($p[$x]['id']|0) . " '" . $p[$x]['name'] . "'<BR>\n";
                }
                next($ret);
              }
            }
            else echo "Empty server<BR>\n";
          break;
          default:
            printf("got unknown message, type=%d, len=%d<BR>",$poo['type'],$poo['length']);
          break;
        }
      }
    }
    else echo "invalid auth challenge message\n";
  }
  else echo "didn't get a valid message from server!\n";


  fclose($fp);
}

$user="user"; // set up the server with a l/p for your status user
$pass="pass";
$port=2051;
$host="localhost";

show_status($host,$port,$user,$pass); 

?>
