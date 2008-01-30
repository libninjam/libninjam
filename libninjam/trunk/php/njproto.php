<?
/// messaging code

function nj_sendmsg($fp, $type, $data)
{
  $len = strlen($data);
  $hdr = chr($type&0xff);
  $hdr .= chr($len&0xff);
  $hdr .= chr(($len>>8)&0xff);
  $hdr .= chr(($len>>16)&0xff);
  $hdr .= chr(($len>>24)&0xff);
  fwrite($fp,$hdr);

  fwrite($fp,$data);
}

function nj_getmsg($fp)
{
  $buf=fread($fp,5);
  if (strlen($buf) != 5) return 0; 

  $ret['type']=ord($buf);
  $len = ord(substr($buf,1));
  $len |= ord(substr($buf,2)) << 8;
  $len |= ord(substr($buf,3)) << 16;
  $len |= ord(substr($buf,4)) << 24;
  
  if ($len < 0 || $len > 16384) return 0;

  if ($len) $buf = fread($fp,$len);
  else $buf="";

  if (strlen($buf) != $len) return 0;

  $ret['length'] = $len;
  $ret['data'] = $buf;
  return $ret;
}

function nj_makepasshash($user, $pass, $challenge)
{
  if (phpversion () < "5") {
    $s = pack("H*", sha1($user . ":" . $pass));
    return (pack("H*", sha1($s . $challenge)));
  } else {
    $s=sha1($user . ":" . $pass,TRUE);
    return sha1($s . $challenge,TRUE);
  }
}

function nj_sendauthmsg($fp, $user, $pass, $challenge)
{
  
  $msg = nj_makepasshash($user,$pass,$challenge);
  $msg .= $user . chr(0);
   
  $caps = 1;
  $ver = 0x00020000;

  $msg .= chr($caps & 0xff);
  $msg .= chr(($caps>>8) & 0xff);
  $msg .= chr(($caps>>16) & 0xff);
  $msg .= chr(($caps>>24) & 0xff);

  $msg .= chr($ver & 0xff);
  $msg .= chr(($ver>>8) & 0xff);
  $msg .= chr(($ver>>16) & 0xff);
  $msg .= chr(($ver>>24) & 0xff);

  nj_sendmsg($fp,0x80,$msg);
}

function nj_parseauthmsg($data)
{
  if (strlen($data) < 4+4+8) { return 0;}
  $ret['challenge'] = substr($data,0,8);

  $ret['servercaps'] = ord(substr($data,8));
  $ret['servercaps'] |= ord(substr($data,9)) << 8;
  $ret['servercaps'] |= ord(substr($data,10)) << 16;
  $ret['servercaps'] |= ord(substr($data,11)) << 24;

  $ret['protover'] = ord(substr($data,12))&0xff;
  $ret['protover'] |= ord(substr($data,13)) << 8;
  $ret['protover'] |= ord(substr($data,14)) << 16;
  $ret['protover'] |= ord(substr($data,15)) << 24;
 
  if ($ret['protover'] < 0x00020000 || $ret['protover'] > 0x0002ffff) { return 0;  }

  $ret['license']="";
  if ($ret['servercaps'] & 1)
  {
    $pos=16;
    while ($pos < strlen($data))
    {
      if (!ord(substr($data,$pos))) 
      {
        $ret['license']=substr($data,16);
    break;
      }
      $pos++;
    }
  }

  return $ret;
}

function nj_parsebpmbpimsg($data)
{
  if (strlen($data) < 4) return 0;
  $ret['bpm']=ord(substr($data,0));
  $ret['bpm']|=ord(substr($data,1))<<8;
  $ret['bpi']=ord(substr($data,2));
  $ret['bpi']|=ord(substr($data,3))<<8;
  return $ret;
}

function nj_parsechatmsg($data)
{
  $len=strlen($data);
  if ($len < 1) return 0;
  $pos=0;
  for ($cnt = 0; $cnt < 16; $cnt ++)
  {
    $lpos=$pos;
    while ($pos < $len && ord(substr($data,$pos,1))) $pos++;
    if ($pos > $lpos) $ret[$cnt] = substr($data,$lpos,$pos-$lpos);
    else $ret[$cnt] = "";
    $pos++;
    if ($pos >= $len) break;
  }
  return $ret;
}

function nj_parseuserinfolist($data)
{
  $len = strlen($data);
  if ($len < 1) return 0;

  $pos = 0;
  $hdrsize=6;
  while ($pos < $len)
  {
    $act=ord(substr($data,$pos,1));
    $idx=ord(substr($data,$pos+1,1));
    $pos += $hdrsize;

    $lpos=$pos;
    while ($pos < $len && ord(substr($data,$pos,1))) $pos++;
    if ($pos > $lpos) $usn = substr($data,$lpos,$pos-$lpos);
    else $usn="";
    $pos++;
    $lpos=$pos;
    while ($pos < $len && ord(substr($data,$pos,1))) $pos++;
    if ($pos > $lpos) $chn = substr($data,$lpos,$pos-$lpos);
    else $chn="";
    $pos++;
    
 
    if ($pos <= $len && $act)
    {
      $ret[$usn][$ret[$usn]['cnt']|0]['id'] = $idx;
      $ret[$usn][$ret[$usn]['cnt']|0]['name'] = $chn;
      $ret[$usn]['cnt']++;
    }
    
  }
  return $ret;
}
?>
