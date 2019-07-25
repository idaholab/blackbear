/**
 * This file will be the replacement for main in the orignal class and ought
 * to retain the same command line functionality, especially if I manage everything
 * properly
 */

#include "SS316ROMPrediction.h"

void lineout (string filename, double (&pass)[number_outputs]);
void writefile (string filename, string header, double (&pass)[number_outputs]);

int main (int argc, char* argv[])
{
    if (argc < 6) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " dt rhom0 rhoi0 vmJ20 evm0 temperature0" << std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }

  string filename = "rom_in.txt";
  string ofilename = "rom_out.txt";

  string argss [number_inputs+2]= {argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]};
  double argvs [number_inputs+2];
  for (unsigned int i = 1 ; i<number_inputs+2 ; ++i)
        argvs[i] = stod(argss[i]);
    //printf ("\nINPUTS:,%.15le,%.15le,%.15le,%.15le,%.15le,%.15le,%.15le,%.15le,%.15le,%.15le", argvs[1], argvs[2], argvs[3], argvs[4], argvs[5], argvs[6], argvs[7], argvs[8], argvs[9], argvs[10]);
  double feains [number_outputs] = {argvs[2],argvs[3],argvs[5]};
  double feadelta [number_outputs] = {0};
  double feaouts [number_outputs];

/// Declare an instance of the class containing the SS316H material specifics to access the compute method
  SS316ROMPrediction _ss316_rom_calculations;
  _ss316_rom_calculations.computeROMPredictions(argvs[1], argvs[2], argvs[3], argvs[4], argvs[5], argvs[6], feadelta[0], feadelta[1], feadelta[2]);
/*  _ss316_rom_calculations.computeROMPredictions(_dt, _mobile_dislocations_old[_qp], _immobile_dislocations_old[_qp], trial_stress_mpa, effective_strain_old, _temperature[_qp], _mobile_dislocation_increment[_qp], _immobile_dislocation_increment[_qp], rom_effective_strain);*/

  // Complete the update to compute the new increment values
  for (unsigned int i = 0 ; i<number_outputs ; ++i)
    feaouts[i] = feadelta[i] + feains[i];

  lineout(ofilename, feaouts);
  return 0;
}

void lineout (string filename, double (&pass)[number_outputs])
{
  int l;
  printf("\nOUTPUT:  ");
  for (l=0;l<number_outputs;++l)
    printf("%.15le ",pass[l]);
  printf("\n");
}

void writefile (string filename, string header, double (&pass)[number_outputs])
{
  int l;
  FILE * file;
  const char * filenamec = filename.c_str();
  const char * headerc = header.c_str();

  file = fopen(filenamec,"a");

  fprintf(file, "\n %s OUTPUT:\n",headerc);
  for (l=0;l<number_outputs;++l)
    fprintf(file, "%.15le, ",pass[l]);

  fprintf(file, "\n");
  fclose(file);
}
