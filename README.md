

# RanDetect
Pattern detection in random strings written in C 

**Usage**
-
 * Compile both Mgen and Mcomp 
	 *	linux: `gcc Mgen.c -o Mgen & gcc Mcomp.c -o Mcomp`
 * Run Mgen and create a model (the sample string needs to be only of 1 and 0)
	 * `./Mgen`
 * Run Mcomp and compare a string to the model
	 * `./Mcomp`

**Understanding the results**
-
 * **Stats**:
	 - Avg abs delta: average of all absolute delta between the model and the string you are comparing it with
	 - Max abs Delta: maximum difference between the model and the string you are comparing it with
	 - Precision: higher this value the less precise the results are
 * **Distance**
	 - Abs distance: sum of all absoulte delta
	 - Aligned distance: abs distance - alignment factor 
	 - Relative distance: aligned distance * precision 
*(the higher these values, the more unlikely it is that the string you are comparing belongs to the model)*

**If you are obtaining imprecise resuts try the following:**
 * First of all make sure your Mgen configuration matches the Mcomp configuration.
 * If the alignment factor, using the default configuration, is too high try with a bigger dataset.
 * If the distance seems too high when comparing to a string that belongs to the model try changing the multipliers.

**Tweaking the configuration**
-
 * `L_CONF_X/L_CONF_Y` and ` L_MOD_X/L_MOD_Y` 
 define the size of the multipliers array and the model array, modifying this values can increase the precision, but can also increase noise, to reduce noise increase the input dataset size and check alignment, if you have a low alignment value you can safely increase this values.
 * `LEN` and `N_STRINGS`define the size of the input dataset (`LEN`) and the size of the string you want to compare it (`LEN/N_STRINGS`).
 * `CONF_MULT`and`MAX_MULT`define the multipliers for the model, we suggest to not modify this values.

**How it works:**

 * Mgen analyzes the distance between repetitions of the same length, then generates multipliers to reduce noise.
 * Mcomp compares the model generated with Mgen and the input string.

**TODO**
-
* Add multiple models support to Mcomp and Mgen
* Add input txt file support
* Add input filters

#

**Note:** Im aware the code is not optimized, ill optimize it for the relase, suggestions and corrections are welcome.


![2](https://github.com/fiustif/RanDetect/assets/40177255/321a3ad4-61bc-4654-b7b5-3069631ae4bc)
![1](https://github.com/fiustif/RanDetect/assets/40177255/ddf37463-7201-4aca-b394-8b54bc36acc6)

