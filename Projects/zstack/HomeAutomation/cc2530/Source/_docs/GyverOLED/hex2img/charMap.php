<?

include("inc/charMap.inc.php");
include("inc/icons_7x7.inc.php");
include("inc/icons_8x8.inc.php");

$f1 = "result/charMap_result.log";
$f2 = "result/icons_7x7_result.log";
$f3 = "result/icons_8x8_result.log";

$data1 = "";
$data2 = "";
$data3 = "";

$idx = 0;
foreach($charMap as $itm) {
	$bin = base_convert($itm, 16, 2);
	$bin = str_pad($bin, 8, '0', STR_PAD_LEFT);
	$bin = str_replace(['0','1'], [' ', '#'], $bin);

	$data1 .= $bin."\n";
	//echo $bin."\n";

	$idx++;
	if($idx % 5 === 0) {
		//echo "--------\n";
		$data1 .= "--------\n";
	}
}

$idx = 0;
foreach($icons_7x7 as $itm) {
	$bin = base_convert($itm, 16, 2);
	$bin = str_pad($bin, 8, '0', STR_PAD_LEFT);
	$bin = str_replace(['0','1'], [' ', '#'], $bin);

	$data2 .= $bin."\n";
	//echo $bin."\n";

	$idx++;
	if($idx % 7 === 0) {
		//echo "--------\n";
		$data2 .= "--------\n";
	}
}

$idx = 0;
foreach($icons_8x8 as $itm) {
	$bin = base_convert($itm, 16, 2);
	$bin = str_pad($bin, 8, '0', STR_PAD_LEFT);
	$bin = str_replace(['0','1'], [' ', '#'], $bin);

	$data3 .= $bin."\n";
	//echo $bin."\n";

	$idx++;
	if($idx % 8 === 0) {
		//echo "--------\n";
		$data3 .= "--------\n";
	}
}

file_put_contents($f1, $data1);
file_put_contents($f2, $data2);
file_put_contents($f3, $data3);
