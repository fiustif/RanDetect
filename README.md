# RanDetect
C program to detect who wrote a random string

**Usage**
-
 * Compile both Mgen and Mcomp 
	 *	linux: `gcc Mgen.c -o Mgen & gcc Mcomp.c -o Mcomp`
 * Run Mgen and create a model (the sample string needs to be only of 1 and 0)
	 * `./Mgen`
 * Run Mcomp and compare a string to the model
	 * `./Mcomp`

**Understanding the results**

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

**How it works:**

 * Mgen analyzes the distance between repetitions of the same length, then generates multipliers to reduce noise.
 * Mcomp compares the model generated with Mgen and the input string.

**Note:** Im aware the code is not optimized, ill optimize it for the relase, suggestions and corrections are welcome.

