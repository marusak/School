<?php
/*Autor:Matej Marusak, VUT FIT 2016, IPP-SYN, proj. 1
#SYN:xmarus06
 * Modul pre spajanie retazcov, ktore vznikaju pri nahradzani tagmi.
 * Ou yes, je to magia, ale to ste este nevideli regexConvertet.php 
 * Princip -> majme 2 retazce, ktore sa lisia len v tagoch
 * 1) Ideme cez retazce, ak su znaky rovnake (a nie <) tak kopirujem do vysledku
 * 2) Ked zacina alebo konci tag iba v jednom, skopirujem tag
 * 3) Ak je v oboch tag, tak jednoducho podla poriadia skopirujem ten spravny (vid dokumentacia)
 */

//zistuje ci zacina tag
function startsTag($str, $i){
    $j = $i;
    if ($str[$i] === "<"){
        while ($i < strlen($str) && $str[$i++] != ">");
        $k = substr($str,$j+1,$i-$j-2);
        switch ($k){
        case "b":
        case "i":
        case "u":
        case "tt":
        case "/b":
        case "/i":
        case "/u":
        case "/tt":
        case "/font":
            return true;
        }
        if (strlen($k) === 11 && substr($k, 0, 10) === "font size=")
            return true;
        if (strlen($k) === 18 && substr($k, 0, 12) === "font color=#")
            return true;
    }    
    return false;
}


function merger($old, $new){
    $res = "";
    $oldI = 0;
    $newI = 0;
    while($oldI < strlen($old) && $newI <strlen($new)){
        if (($old[$oldI] === $new[$newI]) && !startsTag($old, $oldI)){//dva rovnake znaky a nie zacaitok tagu
            $res = $res.$old[$oldI];
            $oldI++;
            $newI++;
        } else if (startsTag($old, $oldI) && !startsTag($new, $newI)){//je tag v prvom, v druhom nie je
            while ($old[$oldI] != ">")//prekopirujeme tag
                $res = $res.$old[$oldI++];
            $res = $res.$old[$oldI++];//+ nezabudneme este >
        } else if (!startsTag($old, $oldI) && startsTag($new, $newI)){//je tag v druhom, v prvom nie je
            while ($new[$newI] != ">")//prekopirujeme tag
                $res = $res.$new[$newI++];
            $res = $res.$new[$newI++];//+ nezabudneme este >
        } else if (startsTag($old,$oldI) && startsTag($new,$newI)){//v oboch su tagy
            //TODO < nemusi byt tag, moze to byt znamineiko mensie...
            if ($new[$newI+1] === "/"){//druhy je koncovy
                //prekopirujem novy
                while ($new[$newI] != ">")//prekopirujeme tag
                    $res = $res.$new[$newI++];
                $res = $res.$new[$newI++];//+ nezabudneme este >
            } else {//prvy je zaciatocny
                //prekopirujem stary
                while ($old[$oldI] != ">")//prekopirujeme tag
                    $res = $res.$old[$oldI++];
                $res = $res.$old[$oldI++];//+ nezabudneme este >
            }
        } else {
            print($old."\n");
            print($new."\n");
            print($oldI." ".$newI."\n");
            error("BUG HAS OCCURED\n",1024);
        }
    }
    // na poradi teraz nezalezi , lebo jeden urcite bude false
    if ($oldI < strlen($old)){//obsahuje este nakonci nejake tagy
        $res = $res.substr($old,$oldI);
    }
    if ($newI < strlen($new)){//obsahuje nakonci este nejake tagy
        $res = $res.substr($new,$newI);
    }
    return ($res);
}

?>
