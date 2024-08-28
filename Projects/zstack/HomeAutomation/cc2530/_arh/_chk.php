<?
$data1 = bindec('00000000');
$data2 = bindec('11111111');
$bit = 5;

//-- set $bit to 1
//$data1 |= (1 << $bit);

//-- set $bit to 0
//$data2 &= ~(1 << $bit);

$data1 |= 0x20;
$data2 |= 0x20;


echo "\n";
echo "76543210\n";
echo "~~~~~~~~\n";
echo str_pad(base_convert($data1, 10, 2), 8, '0', STR_PAD_LEFT)."\n";
echo str_pad(base_convert($data2, 10, 2), 8, '0', STR_PAD_LEFT)."\n";
