Addenda and Corrigenda
======================

Owner's Manual
--------------

**Page 246, Section 3, To define a _softkey_ in the _programmable menu_:**
_for_ <q>_register_ **K**</q> _read_ <q>_register_ **X**</q>,
_for_ <q>the first characters of ***k***</q> _read_ <q>the first characters of ***x***</q>,
_and change_ <q>**K** is not used</q> _to_ <q>**X** is neither used nor dropped</q>.

**Page 247, Section 3, To define a _softkey_ in the _programmable menu_:**
_add_ <q>**X** will be dropped unless you have just defined `▲`, `▼`, or `EXIT`.</q>
_before_ <q>Repeat this procedure</q>.


Reference Manual
----------------

**Pages 100, Purposes of System Flags:**
_for_ <q>NUM.IN, ALP.IN</q> _paint the background green_.


Release note addendum
---------------------

| Version | Release notes                                                                                                                                                                                                                                                                                                                                            |
|---------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0.23.3  | Implemented `DELITM`, `SAVE`/`LOAD` of _named variables_, confirmation of `CLP`, `RBR` for _named variables_, and `A`…`D` and `I`…`L` keys in `RBR`. Reintroduced error 27. Fixed simulator crash when restarted after closed while a browser was open.                                                                                                |
| 0.23.4  | Implemented `FASTFN`, and `CLP` menu. Fixed issue of `DELITM`, `PIXEL`/`AGRAPH` with 399 &lt; x &lt; 400 or 239 &lt; y &lt; 240, string concatenation, `Σ+` memory leakage, `CNST` indirection, and possible crash of `GTO`/`XEQ` after `LOAD`. Optimized some common functions.                                                                         |
| 0.23.5  | Implemented `VARMNU`, `NUM.IN` and `ALP.IN` system flags. Fixed possible crash of `CLP`, `Wp`, and matrix division; issues of `tan` in Multπ mode, `KEYG`/`KEYX`, `DECOMP`, `M.LU`; complex matrix subtraction, eigenpairs, power, and beta function; cube root with `FASTFN` set, `yPx` and `yCx`, special cases of `lnΓ`, `gd`, `P→R`, and `DECOMP`. |
