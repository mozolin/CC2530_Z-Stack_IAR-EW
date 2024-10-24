<?

1729606176
2110153124


$ts = 1729605769;
echo $ts." - ".date("d-m-Y H:i:s", $ts)."\n";

exit;


$t = 1729599585.073;


//$t1 = ($t - strtotime("January 01, 2024 00:00:00 UTC+00:00")) / 1000;
$t1 = $t / 1000;
//$t2 = -180;
$t2 = 0;
$time = round($t1 + $t2);

echo date("d-m-Y H:i:s", time())." (".time().") => ".date("d-m-Y H:i:s", $time)." ($time)\n";
