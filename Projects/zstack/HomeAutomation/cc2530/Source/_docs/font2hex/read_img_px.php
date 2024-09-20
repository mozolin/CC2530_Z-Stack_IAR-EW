<?

$tableNum = 2;


//=====================================================
//=====================================================
//=====================================================


$fn   = "./fonts/font_0".$tableNum.".png";
$fnCh = "./fonts/font_0".$tableNum.".txt";
$fnH  = "./font_0".$tableNum.".h";
$imSrc = imagecreatefrompng($fn);

$w = 16;
$h = 8;

$cArr = [];

$charsList = mb_str_split(file_get_contents($fnCh));
echo $charsList[0]."\n";
echo $charsList[1]."\n";
echo $charsList[2]."\n";

if($imSrc !== false) {

	$sizeSrc = getimagesize($fn);
	if(!empty($sizeSrc[0]) && !empty($sizeSrc[1])) {
		$wSrc = $sizeSrc[0];
		$hSrc = $sizeSrc[1];

		$charsNum = $hSrc / $h;
		
		echo "W:$wSrc, H:$hSrc => $charsNum chars\n\n";

		for($c=0;$c<$charsNum;$c++) {
			
			$im = imagecrop($imSrc, [
				'x' => 0,
				'y' => $c * $h,
				'width' => $w,
				'height' => $h
			]);
			
			if($im !== false) {
	    
				imagepng($im, './fonts/chars/ch_'.str_pad($c, 3, '0', STR_PAD_LEFT).'.png');
				
				$left = [];
				$right = [];
        
				$sRes = '';
				//for($x=0;$x<$w;$x++) {
				for($y=0;$y<$h;$y++) {
					$s = '';
					//for($y=0;$y<$h;$y++) {
					for($x=0;$x<$w;$x++) {
						$rgb = imagecolorat($im, $x, $y);
						$colors = imagecolorsforindex($im, $rgb);
						if($colors['red'] == 255) {
							echo " ";
							$s .= '0';
						} else {
							echo "#";
							$s .= '1';
						}
						//print_r($colors['red']);
					}
					echo "\n";
					//echo $s."\n";
        
					$s1 = "0x".str_pad(base_convert(substr($s,0,8), 2, 16), 2, "0", STR_PAD_LEFT);
					$s2 = "0x".str_pad(base_convert(substr($s,8), 2, 16), 2, "0", STR_PAD_LEFT);
        
					$left[] = $s1;
					$right[] = $s2;
        
					//$sRes .= $s1.",".$s2.",";
					//$sRes .= $s2.",".$s1.",";
					//$sRes .= $s1.",";
					
				}
				
				$sRes .= "  ".implode(",", $right);
				$sRes .= ",";
				$sRes .= implode(",", $left);
				$sRes .= ", //-- #$c: ".$charsList[$c]."\n";
				
				$cArr[] = $sRes;
				
				echo $sRes;
				
				imagedestroy($im);
				//exit;
			}
		}
	}
	
	imagedestroy($imSrc);
}

if(!empty($cArr)) {
	
	$data  = "static const unsigned char font8x16[] =\n";
	$data .= "{\n";
	$data .= implode('', $cArr);
	$data .= "};\n";
	
	file_put_contents($fnH, $data);
}
