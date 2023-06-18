# RanDetect
C program to detect who wrote a random string

Usage:
 * Compile both Mgen and Mcomp
 * Run Mgen and create a model (the sample string needs to be only of 1 and 0)
 * Run Mcomp and compare a string to the model

Understand the results:
 * The Distance is the most significative factor, it indicates the distance between the model and the string you are comparing it with.
 * The stats are only for debugging the model, you can use them to tweak your configuration to achieve better results.

If you are obtaining imprecise resuts try the following:
 * If the allignment factor, using the default configuration, is too high try with a bigger dataset


![1](https://github.com/fiustif/RanDetect/assets/40177255/9b1103fd-a76c-49d3-863c-c24bef52f446)
![2](https://github.com/fiustif/RanDetect/assets/40177255/6d64f934-a3fd-4675-a7b1-ad794deb9913)
