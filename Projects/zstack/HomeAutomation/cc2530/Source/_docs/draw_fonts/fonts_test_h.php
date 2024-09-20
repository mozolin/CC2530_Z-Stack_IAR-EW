<?

include("z_fonts_h.php");

foreach($fonts_h1 as $arr) {
  for($i=0;$i<8;$i++) {
    $item1 = $arr[$i];
    $num1 = substr($item1, 2);
    $n1 = hexdec($num1);
    $str1 = base_convert($num1, 16, 2);
    $str1 = str_pad($str1, 8, "0", STR_PAD_LEFT);
    $str1 = str_replace(['0','1'], ['.','#'], $str1);
    
    echo $str1."\n";
  }
  for($i=0;$i<8;$i++) {
    $item2 = $arr[$i+8];
    $num2 = substr($item2, 2);
    $n2 = hexdec($num2);
    $str2 = base_convert($num2, 16, 2);
    $str2 = str_pad($str2, 8, "0", STR_PAD_LEFT);
    $str2 = str_replace(['0','1'], ['.','#'], $str2);
  
    echo $str2."\n";
  }
  echo "========\n";
}
