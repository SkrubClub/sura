Information for teh devs:

<br>
Str<br>
Agi<br>
Frt<br><br>


Str
<br>
  +Damage<br>
  +Max weight<br><br>


Agi<br>
  +Moves<br>
  +Dodge<br>
  +Crit<br>
Frt<br>
  +Health<br>
  +Regen
<br>
  +Weight<br><br>


Dodge and Crit:
<br>
When an attack lands, both the attacker and defender generate a random integer. These integers have to custom thresholds.<br>
e.g. 128 and 208:<br>
Range 1:	0-127<br>
Range 2:	128-207<br>
Range 3:	208-255<br><br>

Depending on which range the random number lands within, it will add (Range-1)*((boolAttacker*2)-1) to the crit score, i.e. adding if attacking and subtracting if defending.<br><br>


Crit Score:<br>
-2	-1	0	1	2<br>
Miss	Glance	Normal	Crit	Mega Crit
<br>
dmg*0	dmg*0.5	*1	*2	*4