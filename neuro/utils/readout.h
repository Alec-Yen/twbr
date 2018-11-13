/********************************************/
/*Implemented by John Reynolds              */
/*Ported to neuro library on Oct. 18th, 2017*/
/********************************************/
/*This header file provides a reservoir computing style readout layer.*/
/*It implements softmax regression given a state vector from the reservoir.*/
/***************************************************************************/

/*USAGE:*/

/*First, we must initialize the readout layer.*/
/*(1): Readout ro(int inputN, int outputN, double learn_rate);*/

/*Then, we can train it one example at a time (will be expanded) by using the following...*/
/*(2): ro.computeZ(stateMatrix);*/

/*Note that the state matrix is a vector < vector < double > >, but currently only supports a size of one*/
/*I.e. one state vector*/

/*(3): ro.softmax();*/
/*(4): ro.GradientDescent(int correct);*/
/*The argument to GradientDescent is simply the correct label (0-N) for the current instance*/

/*(5): Repeat steps 2-4 for all training instances.*/

/*To use your trained model, you may simply call steps 2 & 3.*/ 
/*If you don't call step 4, weights aren't updated*/
/*Then, check ro.outputY (softmax output) to find the biggest probability.*/
/*You can store the trained weights by calling printWo() and redirecting stdout (or access Wo yourself)*/

#include <cmath>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cfloat>
#include <cstdlib>

#ifndef READOUT_H
#define READOUT_H

double fRand(double fMin, double fMax)
{
	//double f = (double)rand() / RAND_MAX;
	return fMin + ((double)rand()) / (RAND_MAX/(fMax - fMin));
}

namespace NeuroUtils{
	class Readout{
		public:

			/*The constructor will initialize the weight matrix Wo(output)*/
			/*This classifier utilizes softmax regression.*/
			/*This should be #reservoirOutputs by #categories*/
			/*In the case of iris, this is Nx3 */
			Readout(int inputN, int outputN, double learn_rate){

				X_n = 1;

				W_m = inputN;
				W_p = outputN;
				learning_rate = learn_rate;
				
				/*Bengio recommended initial weights*/
				//double r = 4*sqrt(6.0/(inputN+outputN));
				inputZ.resize(outputN, 0);
				bias.resize(W_p, 0);

				std::vector <double> startVec(outputN, 0);
				for(int i=0; i<inputN; i++){
					Wo.push_back(startVec);
					for(int j=0; j<outputN; j++){
						//Wo[i][j] = fRand(-r, r);
						Wo[i][j] = fRand(-0.1, 0.1);
					}
				}

			}

			/*Not implemented*/
			~Readout(){

			}

			std::vector <double> bias;
			std::vector <std::vector <double> > Wo;
			std::vector <std::vector <double> > reservoirOuts;
			std::vector <std::vector <double> > weightGrads;
			std::vector <double> biasGrads;

			/*inputZ represents input to the softmax layer*/
			/*outputY is the output vector of the softmax layer*/
			/*A.K.A. Predictions*/
			std::vector <double> inputZ;
			std::vector <double> outputY;
			std::vector <std::vector <double> > weightGrad;
			int X_n;
			int W_m;
			int W_p;

			/*sumExp is sum of exponentials used in softmax function*/
			double sumExp;
			double learning_rate;

			/*This computes the softmax function.*/
			/*Subtracting Max aids with numerical stability (exp can get large)*/
			void softmax(){
				sumExp = 0;
				outputY.clear();
				double max = *std::max_element(inputZ.begin(), inputZ.end());

				for(int i=0; i<(int)inputZ.size(); i++){
					sumExp+= exp(inputZ[i]-max);
				}

				for(int i=0; i<(int)inputZ.size(); i++){
					outputY.push_back(exp(inputZ[i]-max)/sumExp);
				}

			}
			
			/*Loop over weight matrix, figure out derivative of cross-entropy w.r.t. ij*/
			/*Also update the bias*/
			/*correct is the correct label for the current instance*/
			void GradientDescent(int correct){
				ComputeWeightGradient(correct);
				ComputeBiasGradient(correct);
				for(int i=0; i<(int)Wo.size(); i++){
					for(int j=0; j<(int)Wo[i].size(); j++){
						Wo[i][j] = Wo[i][j] - (learning_rate * weightGrads[i][j]);
					}
				}
				for(int i=0; i<(int)bias.size(); i++){
					bias[i] = bias[i] - (learning_rate * biasGrads[i]);
				}

			}


			/*Compute the Weight Matrix Gradient*/
			void ComputeWeightGradient(int correct){

				std::vector <double> YMinusT;
				for(int i=0; i<(int)outputY.size(); i++){
					if(i == correct)
						YMinusT.push_back(outputY[i] - 1);
					else
						YMinusT.push_back(outputY[i]);

				}

				weightGrads.resize(W_m, std::vector<double>(W_p));
				
				/*Multiply reservoirOuts transposed by the YMinusT vector*/
				for(int i=0; i<W_m; i++){
					for(int j=0; j<X_n; j++){
						for(int k=0; k<W_p; k++){
							weightGrads[i][k] = reservoirOuts[j][i] * YMinusT[k];
//							printf("%f ", weightGrads[i][k]);
						}
//						printf("\n");
					}
				}
			}

			/*Compute the Bias Gradient*/
			void ComputeBiasGradient(int correct){

				std::vector <double> YMinusT;
				for(int i=0; i<(int)outputY.size(); i++){
					if(i == correct)
						YMinusT.push_back(outputY[i] - 1);
					else
						YMinusT.push_back(outputY[i]);
				}

				biasGrads = YMinusT;
				
			}


			void normalize(std::vector <std::vector <double> > &Data){

				double min = DBL_MAX;
				double max = -DBL_MAX;

				for(int i=0; i<(int)Data.size(); i++){
					double tmpMin = *std::min_element(Data[i].begin(), Data[i].end());
					double tmpMax = *std::max_element(Data[i].begin(), Data[i].end());

					if(tmpMin < min)
						min = tmpMin;
					if(tmpMax > max)
						max = tmpMax;
				}

				for(int i=0; i<(int)Data.size(); i++){
					for(int j=0; j<(int)Data[i].size(); j++){	
						Data[i][j] = (Data[i][j]-min)/(max-min);

					}

				}
			}

			/*Multiplies the weights by the reservoir state vector*/
			void computeZ(std::vector <std::vector <double> >reservoirOutput){

				reservoirOuts = reservoirOutput;

				/*for all reservoir outputs and all classes...*/
				/*Multiply the relevant reservoir output by the edge weight*/
				/*Add the result to the related output unit in inputZ*/

				for(int i=0; i<X_n; i++){
					for(int j=0; j<W_p; j++){
						double sum = 0.0;
						for(int k=0; k<W_m; k++){
							sum += reservoirOuts[i][k] * Wo[k][j];
						}
						inputZ[j] = sum;
					}
				}

				/*Add the bias*/
				for(int i=0; i<(int)inputZ.size(); i++){
					inputZ[i] += bias[i];
				}
			}

			/*Given the correct label, what's the cross-entropy loss?*/
			double CrossEntropy(int correct){

				double result = 0.0;
				double val = 0.0;	
				for(int i=0; i<(int)outputY.size(); i++){
					val = outputY[i];
					if(outputY[i] == 0){
						val = 0.000001;
					}
					if(outputY[i] == 1){
						val = 0.999999;
					}
					if(i == correct){
							result += log(val);
					}
					else{
						result += log(1 - val);
					}
				}

				return -result;
			}

			/*Print the weight matrix*/
			void printWo(){
				printf("%d %d\n", (int)Wo.size(), (int)Wo[0].size());
				for(int i=0; i<(int)Wo.size(); i++){
					for(int j=0; j<(int)Wo[i].size(); j++){
						printf("%f ", Wo[i][j]);
					}
					printf("\n");
				}
			}

			/*This function exports the stored weights*/
			std::string export_weights(){

				std::ostringstream os;
				
				os << Wo.size() << " " << Wo[0].size() << "\n";
				for(int i=0; i<(int)Wo.size(); i++){
					for(int j=0; j<(int)Wo[i].size(); j++){
						os << Wo[i][j] << " ";
					}
					os << "\n";
				}

				return os.str();
			}

			/*This function sets the weight matrix from a */
			/*string in the export_weights format*/
			void import_weights(std::string weight_string){
				
				std::istringstream is(weight_string);

				int h = 0, w = 0;

				is >> h >> w;

				for(int i=0; i<h; i++){
					for(int j=0; j<w; j++){
						is >> Wo[i][j]; 
					}
				}

				is.clear();
			}

			/*Print the result of state * Wo */
			void printZ(){
				printf("Printing Z\n");
				for(int i=0; i<(int)inputZ.size(); i++){
					printf("%f ", inputZ[i]);
				}
				printf("\n");
			}

			/*Print softmax output*/
			void printY(){
				for(int i=0; i<(int)outputY.size(); i++){
					printf("%f ", outputY[i]);
				}
				printf("\n");
			}
	};
}
#endif
