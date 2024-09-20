<?
$string = 'ABCDЯ';

for($i = 0; $i < strlen($string); $i++) {
    echo mb_substr($string, $i, 1, 'UTF-8') . "\n";
}
