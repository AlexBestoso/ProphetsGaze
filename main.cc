#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

using namespace std;


class ProphetsGaze{
	private:
		string key = "";
		string keyOperator = "";
		string message = "";
		int *result = NULL;
		size_t resultSize = -1;

		size_t sBoxSize = 57;
		
		char sBox[57] = {
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
			'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
			'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '.', '!', '?', '`', ',', ':', ';', '(',
			')', '"', '\'', 0x47, 0x48, 0x49, 0x50, 0x51, 0x52, 0x53,
			0x54, 0x55, ' '
		};

		char getSboxChar(int x){
			for(int i=0; i<this->sBoxSize; i++){
				if(i == x){
					return this->sBox[i];
				}	
			}
			fprintf(stderr, "Bad Data In Sbox %d\n", x);
			return 0x00;
		}

		int getSboxValue(char x){
			for(int i=0; i<this->sBoxSize; i++){
				if(x == this->sBox[i]){
					return i;
				}
			}
			return -1;
		}

		void makeLowerCase(){
			string tmp = "";
			for(int i=0; i<this->message.length(); i++){
				tmp += tolower(this->message[i]);
			}
			this->setMessage(tmp);
		}

		void inv_substitute(){
			this->message = "";
			for(int i=0; i<this->resultSize; i++){
				char unSubbed = this->getSboxChar(this->result[i]);
				if(unSubbed == 0x47)
					message += "ll";
				else if(unSubbed == 0x48)
					message += "th";
				else if(unSubbed == 0x49)
					message += "ed";
				else if(unSubbed == 0x50)
					message += "ing";
				else if(unSubbed == 0x51)
				       	message += "ch";
				else if(unSubbed == 0x52)
					message += "mm"; 
				else if(unSubbed == 0x53)
					message += "tt";
				else if(unSubbed == 0x54)
					message += "ss"; 
				else if(unSubbed == 0x55)
					message += "ee";
				else
					message += unSubbed;
			}
		}

		bool substitute(){
			if(this->message == ""){
				fprintf(stderr, "Message is empty\n");
				return false;
			}

			this->makeLowerCase();

			/*
			 * Substitute letter combos
			 * */
			string tmp = "";
			for(int i=0; i<this->message.length(); i++){
				if(i < this->message.length()-1 || i < this->message.length()-2){
					if(this->message[i] == 'l' && this->message[i+1] == 'l'){
						tmp += 0x47;
						i += 1;
					}else if(this->message[i] == 't' && this->message[i+1] == 'h'){
						tmp += 0x48;
						i += 1;
					}else if(this->message[i] == 'e' && this->message[i+1] == 'd'){
						tmp += 0x49;
						i += 1;
					}else if(i < this->message.length()-2 && (this->message[i] == 'i' && this->message[i+1] == 'n' && this->message[i+2] == 'g')){
						tmp += 0x50;
						i += 2;
					}else if(this->message[i] == 'c' && this->message[i+1] == 'h'){
						tmp += 0x51;
						i += 1;	
					}else if(this->message[i] == 'm' && this->message[i+1] == 'm'){
						tmp += 0x52;
						i += 1;
					}else if(this->message[i] == 't' && this->message[i+1] == 't'){
						tmp += 0x53;
						i += 1;
					}else if(this->message[i] == 's' && this->message[i+1] == 's'){
						tmp += 0x54;
						i += 1;
					}else if(this->message[i] == 'e' && this->message[i+1] == 'e'){
						tmp += 0x55;
						i += 1;
					}else{
						tmp += this->message[i];
					}
				}else{
					tmp += this->message[i];
				}
			}
			this->message = tmp;
			size_t substitueArraySize = this->message.length();
			int sArray[substitueArraySize];
                   
			int o = 0;
			for(int i=0; i<this->message.length(); i++){
				bool set = false;
				for(int j=0; j<this->sBoxSize; j++){
					if(this->message[i] == this->sBox[j]){
						sArray[i] = j;
						set = true;
					}
				}
				if(!set){
					sArray[i] = -1;
				}
			}

			this->resultSize = substitueArraySize;
			this->result = (int *)malloc(sizeof(int) * this->resultSize);
			for(int i=0; i<substitueArraySize; i++)
				result[i] = sArray[i];
		}

		void keyShift(){
			int shiftAmount = (this->key.length() * this->message.length()) % sBoxSize;
				
			for(int i=0; i<this->resultSize; i++){
				if(this->result[i] <= -1){
					continue;	
				}
				int q = this->result[i] + shiftAmount;
				int o = q / sBoxSize;
				this->result[i] = q - (o * sBoxSize);
			}
		}

		void inv_keyShift(){
                        int shiftAmount = (this->key.length() * this->message.length()) % sBoxSize;

                        for(int i=0; i<this->resultSize; i++){
                                if(this->result[i] <= -1){
                                        continue;
                                }
                                int q = this->result[i] - shiftAmount;
                                int o = q / sBoxSize;
				if(o < 0 || o > sBoxSize)
					o = 0;
				if(q >= 0)
                                	this->result[i] = q - (o * sBoxSize);
				else{
					this->result[i] = sBoxSize + (q - (o * sBoxSize));
				}

                        }
                }

		bool expandKey(int blockEndPos){
			int keyLen = this->keyOperator.length();
			int blockStartPos = blockEndPos - keyLen;
			if(blockStartPos < 0)
				return false;

			int keyI = 0;
			string newKey = "";
			for(int i=blockStartPos; i<blockEndPos; i++){
				if(keyI >= this->keyOperator.length()){
					fprintf(stderr, "Failed to expand key\n");
					return false;
				}
				int a = this->result[i] + 17;
				int b = (this->getSboxValue(this->keyOperator[keyI]) + 23) * 7;
				int q = a * b;
                                int o = q / sBoxSize;
				
				if(q >= 0)
                                	newKey += this->getSboxChar(q - (o * sBoxSize));
				else
					newKey += this->getSboxChar(sBoxSize - (q - (o * sBoxSize)));
				keyI++;
			}
			this->keyOperator = newKey;
			return true;
		}

		void inv_rightwardApplyKey(){
			bool expandKey = false;
			int tmp[this->resultSize];
			this->keyOperator = this->key;

			for(int i=0; i<this->resultSize; i++)
				tmp[i] = this->result[i];

                        for(int i=0; i<this->resultSize; i++){
                                if(expandKey){
                                        if(!this->expandKey(i))
                                                fprintf(stderr, "Bad key Expansion\n");
                                        expandKey = false;
                                }

                                if(i % this->keyOperator.length() == this->keyOperator.length() - 1)
                                        expandKey = true;

                                if((i%2) == 0){
                                        // inv shift right
                                        int q = tmp[i] - this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                                o = 0;
                                        if(q < 0){
                                                tmp[i] = sBoxSize + (q - (o * sBoxSize));
                                        }else{
                                                tmp[i] = q - (o * sBoxSize);
                                        }
                                }else{
                                        // inv shift left
                                        int q = tmp[i] + this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                                o = 0;
                                        if(q < 0){
                                                tmp[i] = sBoxSize + (q - (o * sBoxSize));
                                        }else{
                                                tmp[i] = q - (o * sBoxSize);
                                        }
                                }
                        }
			/*
			 * Undo permutation.
			 * */
			for(int i=0; i<this->resultSize; i++)
				this->result[i] = tmp[i];

			size_t rows = this->resultSize / 7;
                        if(rows*7 < this->resultSize){
                                rows++;
                        }
                        int trey[7][rows];
                        int rowI = -1;

                        for(int i=0; i<rows; i++)
                                for(int j=0; j<7; j++)
                                        trey[j][i] = -2;

                        for(int i=0; i<this->resultSize; i++){
                                if(i%7 == 0)
                                        rowI++;
                                trey[i%7][rowI] = this->result[i];
                        }

                        int tmpI = 0;
                        for(int i=0; i<rows; i++){
                                if((i%2 == 0)){
                                        // rightward
                                        for(int j=0; j<7; j++){
                                                 if(trey[j][i] == -2)
                                                        continue;
                                                this->result[tmpI] = trey[j][i];
                                                tmpI++;
                                        }
                                }else{
                                        //leftward
                                        for(int j=6; j>-1; j--){
                                                if(trey[j][i] == -2)
                                                        continue;
                                                this->result[tmpI] = trey[j][i];
                                                tmpI++;
                                        }
                                }
                        }

		}


		void rightwardApplyKey(){
			this->keyOperator = this->key;
			size_t rows = this->resultSize / 7;
			if(rows*7 < this->resultSize){
				rows++;
			}
			int trey[7][rows];
			int rowI = -1;
			
			for(int i=0; i<rows; i++)
				for(int j=0; j<7; j++)
					trey[j][i] = -2;

			for(int i=0; i<this->resultSize; i++){
				if(i%7 == 0)
					rowI++;
				trey[i%7][rowI] = this->result[i];
			}
			
			/*
			 * alighn the buffer left to right snake.
			 * */
			int tmp[this->resultSize];
			int tmpI = 0;
			for(int i=0; i<rows; i++){
				if((i%2 == 0)){
					// rightward
					for(int j=0; j<7; j++){
						 if(trey[j][i] == -2)
                                                        continue;
						tmp[tmpI] = trey[j][i];
						tmpI++;
					}
				}else{
					//leftward
					for(int j=6; j>-1; j--){
						if(trey[j][i] == -2)
							continue;
						tmp[tmpI] = trey[j][i];
						tmpI++;
					}
				}
			}

			/*
			 * apply and expand key
			 * */
			bool expandKey = false;
			for(int i=0; i<this->resultSize; i++){
				if(expandKey){

					if(!this->expandKey(i))
						fprintf(stderr, "Bad key Expansion\n");
					expandKey = false;
				}

				if(i % this->keyOperator.length() == this->keyOperator.length() - 1)
					expandKey = true;

				if((i%2) == 0){
					// shift right
					int q = tmp[i] + this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                	int o = q / sBoxSize;
                                	this->result[i] = q - (o * sBoxSize);
				}else{
					// shift left
					int q = tmp[i] - this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                        	o = 0;
					if(q < 0){
						this->result[i] = sBoxSize + (q - (o * sBoxSize));
					}else{
						this->result[i] = q - (o * sBoxSize);
					}
				}
			}
			

		}


		void inv_leftwardApplyKey(){
			bool expandKey = false;
			int tmp[this->resultSize];
			this->keyOperator = this->key;

			for(int i=0; i<this->resultSize; i++)
				tmp[i] = this->result[i];

                        for(int i=0; i<this->resultSize; i++){
                                if(expandKey){
                                        if(!this->expandKey(i))
                                                fprintf(stderr, "Bad key Expansion\n");
                                        expandKey = false;
                                }

                                if(i % this->keyOperator.length() == this->keyOperator.length() - 1)
                                        expandKey = true;

                                if((i%2) == 0){
                                        // inv shift right
                                        int q = tmp[i] - this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                                o = 0;
                                        if(q < 0){
                                                tmp[i] = sBoxSize + (q - (o * sBoxSize));
                                        }else{
                                                tmp[i] = q - (o * sBoxSize);
                                        }
                                }else{
                                        // inv shift left
                                        int q = tmp[i] + this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                                o = 0;
                                        if(q < 0){
                                                tmp[i] = sBoxSize + (q - (o * sBoxSize));
                                        }else{
                                                tmp[i] = q - (o * sBoxSize);
                                        }
                                }
                        }
			/*
			 * Undo permutation.
			 * */
			for(int i=0; i<this->resultSize; i++)
				this->result[i] = tmp[i];

			size_t rows = this->resultSize / 7;
                        if(rows*7 < this->resultSize){
                                rows++;
                        }
                        int trey[7][rows];
                        int rowI = -1;

                        for(int i=0; i<rows; i++)
                                for(int j=0; j<7; j++)
                                        trey[j][i] = -2;

                        for(int i=0; i<this->resultSize; i++){
                                if(i%7 == 0)
                                        rowI++;
                                trey[i%7][rowI] = this->result[i];
                        }

                        int tmpI = 0;
                        for(int i=0; i<rows; i++){
                                if((i%2 == 1)){
                                        // rightward
                                        for(int j=0; j<7; j++){
                                                 if(trey[j][i] == -2)
                                                        continue;
                                                this->result[tmpI] = trey[j][i];
                                                tmpI++;
                                        }
                                }else{
                                        //leftward
                                        for(int j=6; j>-1; j--){
                                                if(trey[j][i] == -2)
                                                        continue;
                                                this->result[tmpI] = trey[j][i];
                                                tmpI++;
                                        }
                                }
                        }

		}
		void leftwardApplyKey(){
			this->keyOperator = this->key;
			size_t rows = this->resultSize / 7;
			if(rows*7 < this->resultSize){
                                rows++;
                        }
			int trey[7][rows];
			int rowI = -1;
			
			for(int i=0; i<rows; i++){
				for(int j=0; j<7; j++){
					trey[j][i] = -2;
				}
			}

			for(int i=0; i<this->resultSize; i++){
				if(i%7 == 0){
					rowI++;
				}
				trey[i%7][rowI] = this->result[i];
			}
			
			/*
			 * alighn the buffer left to right snake.
			 * */
			int tmp[this->resultSize];
			int tmpI = 0;
			for(int i=0; i<rows; i++){
				if((i%2 == 1)){
					// rightward
					for(int j=0; j<7; j++){
						if(trey[j][i] == -2)
                                                        continue;
						tmp[tmpI] = trey[j][i];
						tmpI++;
					}
				}else{
					//leftward
					for(int j=6; j>-1; j--){
						if(trey[j][i] == -2)
							continue;
						tmp[tmpI] = trey[j][i];
						tmpI++;
					}
				}
			}

			/*
			 * apply and expand key
			 * */
			bool expandKey = false;
			for(int i=0; i<this->resultSize; i++){
				if(expandKey){
                                        if(!this->expandKey(i))
                                                fprintf(stderr, "Bad key Expansion\n");
					expandKey = false;
                                }

                                if(i % this->keyOperator.length() == this->keyOperator.length() - 1)
                                        expandKey = true;

				if((i%2) == 0){
					// shift right
					int q = tmp[i] + this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                	int o = q / sBoxSize;
                                	this->result[i] = q - (o * sBoxSize);
				}else{
					// shift left
					int q = tmp[i] - this->getSboxValue(this->keyOperator[i%this->keyOperator.length()]);
                                        int o = q / sBoxSize;
					if(o < 0 || o > sBoxSize)
                                                o = 0;
					if(q < 0){
						this->result[i] = sBoxSize + (q - (o * sBoxSize));
					}else{
						this->result[i] = q - (o * sBoxSize);
					}
				}
			}
		}

	public:

		ProphetsGaze(){}
		ProphetsGaze(string key, string message){
			this->setKey(key);
			this->setMessage(message);
		}
		size_t getResultSize(){
			return this->resultSize;
		}
		int *getResult(){
			return this->result;
		}
		void setKey(string key){
			this->key = key;
		}
		void setMessage(string message){
			this->message = message;
		}

		void decipher(){
			this->resultSize = this->message.length();
			this->result = (int *)malloc(sizeof(int)*this->resultSize);
			for(int i=0; i<this->resultSize; i++)
				this->result[i] = this->getSboxValue(this->message[i]);
			
			for(int i=0; i<2; i++){
				this->inv_rightwardApplyKey();
				this->inv_leftwardApplyKey();
				this->inv_rightwardApplyKey();
				this->inv_keyShift();
			}
			this->inv_substitute();

			printf("message : %s\n", this->message.c_str());
		}

		void encipher(){
			this->substitute();
			for(int i=0; i<2; i++){
				this->keyShift();
				this->rightwardApplyKey();
				this->leftwardApplyKey();
				this->rightwardApplyKey();
			}

			printf("sboxed : --> ");
			for(int i=0; i<this->resultSize; i++){
				printf("%c", this->getSboxChar(this->result[i]));
			}printf(" <--\n");

			printf("Numerical : ");
			for(int i=0; i<this->resultSize; i++){
                                printf("%d ", this->result[i]);
                        }printf("\n");
		}
};

int main(int argc, char *argv[]){
	if(argc != 4){
		fprintf(stderr, "Usage : %s [-e/-d] [key] [message]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	string mode = argv[1]; 
	ProphetsGaze pg(argv[2], argv[3]);

	
	if(mode == "-d"){
		printf("Decrypt mode\n");
		pg.decipher();
	}else if(mode == "-e"){
		printf("Enciphering Message...\n");
		pg.encipher();
	}else{
		fprintf(stderr, "Invalid Encryption Mode.\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
