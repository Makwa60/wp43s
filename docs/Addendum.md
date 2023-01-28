Addenda and Corrigenda
======================

Owner's Manual
--------------

**Page 121, Section 2, Real Numbers: Some Industrial Problems Solved:**
_for all 3 occurrences of_ <q>0.006</q> _read_ <q>−0.006</q>,
_for_ <q>`−`</q> _at the left of_ <q>12.346 = lower limit</q> _read_ <q>`+`</q>,
_and for_ <q>2 `×` `+`</q> _read_ <q>2 `×` `−`</q>.

**Page 282, Section 5, `.`:**
_change_ <q>another `.` returns to the 1st screen of RBR as shown above</q>
_to_ <q>another `.` turns to _named variables_ (including system variables), and one more `.` returns to the 1st screen of RBR as shown above</q>.

**Page 283, Section 5, `00` … `99`:**
_add_ <q>If pressed while _named variables_ are displayed, it jumps to the corresponding _global register_.</q>

Reference Manual
----------------

**Page 94, Names of System Variables and System Flags:**
_add:_

| _Name_ | Keystrokes | Remarks                            |
|--------|------------|------------------------------------|
| FASTFN |            | _System flags_ – see next chapter. |

**Pages 96 to 100, Purposes of System Flags:**
_add:_

| Purpose         | _SBI_ | _Flag_ | Remarks                                                                                                                                                                                 |
|-----------------|:-----:|:------:|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Faster function |   —   | FASTFN | If set, your _WP43_ calculates faster without using internal float of extended precision. This is a tradeoff between speed and precision. Affects $\sqrt x$, sin, cos, tan, and arctan. |

**Page 177, Appendix C, error 27:**
_add_ <q>Cannot delete a predefined item</q> _in the leftmost blank field,
and change its explanations in the rightmost field to_ <q>Self-explanatory</q>.
_It should read:_

|                                 | _EC_ | Explanations, countermeasures and examples |
|---------------------------------|:----:|--------------------------------------------|
| Cannot delete a predefined item |  27  | Self-explanatory.                          |

Release note addendum
---------------------

| Version | Release notes                                                                                                                                                                                                                                             |
|---------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0.23.3  | Implemented `DELITM`, `SAVE`/`LOAD` of _named variables_, confirmation of `CLP`, `RBR` for _named variables_, and `A`…`D` and `I`…`L` keys in `RBR`. Reintroduced error 27. Fixed simulator crash when restarted after closed while a browser was open. |
