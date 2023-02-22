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

**Page 246, Section 3, To define a _softkey_ in the _programmable menu_:**
_for_ <q>_register_ **K**</q> _read_ <q>_register_ **X**</q>,
_for_ <q>the first characters of ***k***</q> _read_ <q>the first characters of ***x***</q>,
_and change_ <q>**K** is not used</q> _to_ <q>**X** is neither used nor dropped</q>.

**Page 247, Section 3, To define a _softkey_ in the _programmable menu_:**
_add_ <q>**X** will be dropped unless you have just defined `▲`, `▼`, or `EXIT`.</q>
_before_ <q>Repeat this procedure</q>.

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

**Page 343, Appendix 4: Graphics, AGRAPH:**
_for_ <q>1 ≤ ***x*** ≤ 400 and 1 ≤ ***y*** ≤ 186</q> _read_ <q>0 ≤ ***x*** &lt; 400 and 0 ≤ ***y*** &lt; 240</q>.

Reference Manual
----------------

**Page 17, `AGRAPH`:**
_for_ <q>0 ≤ ***x*** ≤ 399 and 0 ≤ ***y*** ≤ 175</q> _read_ <q>0 ≤ ***x*** &lt; 400 and 0 ≤ ***y*** &lt; 240</q>.

**Page 24, `CLP`:**
_for_ <q>`CLR` CLP</q> _read_ <q>`CLR` CLP ***labl***</q>,
_and for_ <q>the _current <u>program</u>_</q> _read_ <q>specified program</q>.

**Page 32, `erf` and `erfc`:**
_for_ <q>\{2\}; \{1\} → \{2\}</q> _read_ <q>\{2, 8*\}; \{1\} → \{2\}</q>.

**Page 33, `EXPT`:**
_for_ <q>\{1, 2\} → \{1\}</q> _read_ <q>\{1, 8*\}; \{2\} → \{1\}</q>.

**Page 33, `FIB`:**
_for_ <q>\{2, 3\}</q> _read_ <q>\{2, 3, 8*, 9*\}</q>.

**Page 47, `MANT`:**
_for_ <q>\{2\}; \{1\} → \{2\}</q> _read_ <q>\{2, 8*\}; \{1\} → \{2\}</q>.

**Page 76, `Wm`, `Wp`, and `W⁻¹`:**
_for_ <q>\{2, 3\}; \{1\} → \{2\}</q> _read_ <q>\{2, 3, 8*, 9*\}; \{1\} → \{2\}</q>.

**Page 79, `x!`:**
_for_ <q>\{2, 3\}</q> _read_ <q>\{2, 3, 8*, 9*\}</q>.

**Page 83, `ζ(x)`:**
_for_ <q>\{2, 3\}</q> _read_ <q>\{2, 3, 8*, 9*\}</q>.

**Page 89, `|x|`:**
_delete_ <q>or `PART` \|x\| or `CPX` \|x\|</q>.

**Page 94, Names of System Variables and System Flags:**
_add:_

| _Name_ | Keystrokes | Remarks                            |
|--------|------------|------------------------------------|
| FASTFN |            | _System flags_ – see next chapter. |

**Pages 96 to 100, Purposes of System Flags:**
_add:_

| Purpose         | _SBI_ | _Flag_ | Remarks                                                                                                                                                                                                                                                                                                                                                                   |
|-----------------|:-----:|:------:|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Faster function |   —   | FASTFN | If set, your _WP43_ calculates faster without using internal float of extended precision. This is a tradeoff between speed and precision. Affects $\sqrt x$, $\sqrt[3]{x}$, ln, $\ln \left( 1 + x \right)$, lg, lb, $\log_x y$, $e^x$, $e^x-1$, $10^x$, $2^x$, $y^x$, $\sqrt[x]{y}$, sin, cos, tan, arcsin, arccos, arctan, sinh, cosh, tanh, arsinh, arcosh, and artanh. |

**Pages 100, Purposes of System Flags:**
_for_ <q>NUM.IN, ALP.IN</q> _paint the background green_.

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

| Version | Release notes                                                                                                                                                                                                                                                                                                                                            |
|---------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0.23.3  | Implemented `DELITM`, `SAVE`/`LOAD` of _named variables_, confirmation of `CLP`, `RBR` for _named variables_, and `A`…`D` and `I`…`L` keys in `RBR`. Reintroduced error 27. Fixed simulator crash when restarted after closed while a browser was open.                                                                                                |
| 0.23.4  | Implemented `FASTFN`, and `CLP` menu. Fixed issue of `DELITM`, `PIXEL`/`AGRAPH` with 399 &lt; x &lt; 400 or 239 &lt; y &lt; 240, string concatenation, `Σ+` memory leakage, `CNST` indirection, and possible crash of `GTO`/`XEQ` after `LOAD`. Optimized some common functions.                                                                         |
| 0.23.5  | Implemented `VARMNU`, `NUM.IN` and `ALP.IN` system flags. Fixed possible crash of `CLP`, `Wp`, and matrix division; issues of `tan` in Multπ mode, `KEYG`/`KEYX`, `DECOMP`, `M.LU`; complex matrix subtraction, eigenpairs, power, and beta function; cube root with `FASTFN` set, `yPx` and `yCx`, special cases of `lnΓ`, `gd`, `P→R`, and `DECOMP`. |
