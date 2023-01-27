Addenda and Corrigenda
======================

Owner's Manual
--------------

**Page 55, Section 1, Clearing and Resetting Your _WP43_, `CLP`:**
_for_ <q>Clears the _current program_</q> _read_ <q>Clears the program specified</q>.

**Page 121, Section 2, Real Numbers: Some Industrial Problems Solved:**
_for all 3 occurrences of_ <q>0.006</q> _read_ <q>−0.006</q>,
_for_ <q>`−`</q> _at the left of_ <q>12.346 = lower limit</q> _read_ <q>`+`</q>,
_and for_ <q>2 `×` `+`</q> _read_ <q>2 `×` `−`</q>.

**Page 250, Section 3, Deleting Programs:**
_change_ <q>move the program pointer into this program
(e.g. by entering `GTO` `.` and picking the label of this program),
then press `CLR` CLP.</q>,
_to_ <q>press `CLR` CLP, then press PROG to choose the program,
or press `α` to input the program name,
or alternatively press `ENTER` to delete the _current program_.</q>.

**Page 251, Section 3, Flash Memory (_FM_):**
_for all 3 occurrences of_ <q>`CLR` CLP</q> _read_ <q>`CLR` CLP `ENTER`</q>.

**Page 282, Section 5, `.`:**
_change_ <q>another `.` returns to the 1st screen of RBR as shown above</q>
_to_ <q>another `.` turns to _named variables_ (including system variables), and one more `.` returns to the 1st screen of RBR as shown above</q>.

**Page 283, Section 5, `00` … `99`:**
_add_ <q>If pressed while _named variables_ are displayed, it jumps to the corresponding _global register_.</q>

Reference Manual
----------------

**Page 24, `CLP`:**
_for_ <q>`CLR` CLP</q> _read_ <q>`CLR` CLP ***labl***</q>,
_and for_ <q>the _current <u>program</u>_</q> _read_ <q>specified program</q>.

**Page 177, Appendix C, error 27:**
_add_ <q>Cannot delete a predefined item</q> _in the leftmost blank field,
and change its explanations in the rightmost field to_ <q>Self-explanatory</q>.
_It should read:_

|                                 | _EC_ | Explanations, countermeasures and examples |
|---------------------------------|:----:|--------------------------------------------|
| Cannot delete a predefined item |  27  | Self-explanatory.                          |

**Page Q-6, WP43 Quick Reference Guide, CLEARING AND DELETING, `CLP`:**
_for_ <q>the _current program_</q> _read_ <q>the program specified</q>.

Release note addendum
---------------------

| Version | Release notes                                                                                                                                                                                                                                             |
|---------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0.23.3  | Implemented `DELITM`, `SAVE`/`LOAD` of _named variables_, confirmation of `CLP`, `RBR` for _named variables_, and `A`…`D` and `I`…`L` keys in `RBR`. Reintroduced error 27. Fixed simulator crash when restarted after closed while a browser was open. |
