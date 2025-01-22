#include "Serial.h"

extern void OutStr(char *str);
// Define the functions
static void printInt(int number) {
    int divFactor=10;
		int modLeft=0;
		int divResult=0;
		char textTemp[12];
		char text[12];
		int negativeFlag=0;
		int counter=0;
		while(1){
			if(number<0 && negativeFlag==0){
				text[0]='-';
				negativeFlag=1;
			}
			divResult=number/divFactor;
			if(negativeFlag){
				modLeft=-(number+(divResult*divFactor));
			}
			else{
				modLeft=number-(divResult*divFactor);
			}
			textTemp[counter++]='0'+modLeft;
			number=number/divFactor;
			if(number==0){
				if(negativeFlag){
					for(int i=0;i<counter;i++){
						text[i+1]=textTemp[counter-i-1];
					};
				}
				else{
					for(int i=0;i<counter;i++){
						text[i]=textTemp[counter-i-1];
					};
				}
				text[counter]=0x04;
				break;
			}
		}
		OutStr(text);
}

void printlnStr(char str[]) {
    int length=0;
		while (str[length] != '\0') {
        length++;
    }
		char text[length+2];
		for (int i=0;i<length;i++){
			text[i]=str[i];
		}
		text[length]='\r';
		text[length+1]='\n';
		text[length+2]=0x04;
		OutStr(text);
		
}



static void printlnInt(int number) {
   int divFactor=10;
		int modLeft=0;
		int divResult=0;
		char textTemp[12];
		char text[12];
		int negativeFlag=0;
		int counter=0;
		while(1){
			if(number<0 && negativeFlag==0){
				text[0]='-';
				negativeFlag=1;
			}
			divResult=number/divFactor;
			if(negativeFlag){
				modLeft=-(number+(divResult*divFactor));
			}
			else{
				modLeft=number-(divResult*divFactor);
			}
			textTemp[counter++]='0'+modLeft;
			number=number/divFactor;
			if(number==0){
				if(negativeFlag){
					for(int i=0;i<counter;i++){
						text[i+1]=textTemp[counter-i-1];
					};
				}
				else{
					for(int i=0;i<counter;i++){
						text[i]=textTemp[counter-i-1];
					};
				}
				text[counter++]='\r';
				text[counter++]='\n';
				text[counter]=0x04;
				break;
			}
		}
		OutStr(text);
}

void printStr(char *str) {
		int length=0;
		while (str[length] != '\0') {
        length++;
    }
		char text[length+1];
		for (int i=0;i<length;i++){
			text[i]=str[i];
		}
		text[length]='\r';
		text[length+1]=0x04;
		OutStr(text);
}

static void printlnFloat(float number, int decimalPlaces) {
    unsigned int result = 1;
    for (int i = 0; i < decimalPlaces; i++) {
        result *= 10;
    }

    char text[20];
    int integerPart = (int)number;
    float fractionalPart = number - (float)integerPart;

    if (fractionalPart < 0) {
        fractionalPart = -fractionalPart; // Make fractional part positive
    }

    // Apply rounding to the fractional part
    int fractionalDigits = (int)((fractionalPart * result) + 0.5);

    int index = 0;

    // Add negative sign if needed
    if (number < 0) {
        text[index++] = '-';
    }

    // Convert integer part to string
    int intTemp = (integerPart < 0) ? -integerPart : integerPart;
    char intBuffer[12];
    int intIndex = 0;
    if (intTemp == 0) {
        intBuffer[intIndex++] = '0';
    } else {
        while (intTemp > 0) {
            intBuffer[intIndex++] = '0' + (intTemp % 10);
            intTemp /= 10;
        }
    }

    // Reverse and add integer part
    for (int i = intIndex - 1; i >= 0; i--) {
        text[index++] = intBuffer[i];
    }

    // Add decimal point
    text[index++] = '.';

    // Convert fractional part to string
    char fracBuffer[12];
    int fracIndex = 0;
    if (fractionalDigits == 0) {
        fracBuffer[fracIndex++] = '0';
    } else {
        while (fractionalDigits > 0) {
            fracBuffer[fracIndex++] = '0' + (fractionalDigits % 10);
            fractionalDigits /= 10;
        }
    }

    // Add leading zeros for smaller fractions
    for (int i = fracIndex; i < decimalPlaces; i++) {
        text[index++] = '0';
    }

    // Reverse and add fractional part
    for (int i = fracIndex - 1; i >= 0; i--) {
        text[index++] = fracBuffer[i];
    }

    // End with newline and termination
    text[index++] = '\r';
    text[index++] = '\n';
    text[index] = 0x04;

    // Send the string
    OutStr(text);
}


static void printFloat(float number, int decimalPlaces) {
		unsigned int result = 1;
    for (int i = 0; i < decimalPlaces; i++) {
        result *= 10;
    }
    char text[20];
    int integerPart = (int)number;
    float fractionalPart = number - (float)integerPart;
    if (fractionalPart < 0) {
        fractionalPart = -fractionalPart;
    }

    
    // Apply rounding to the fractional part
    int fractionalDigits = (int)((fractionalPart * result) + 0.5);

    int index = 0;

    if (number < 0) {
        text[index++] = '-';
    }

    // Convert integer part
    int intTemp = (integerPart < 0) ? -integerPart : integerPart;
    char intBuffer[12];
    int intIndex = 0;
    if (intTemp == 0) {
        intBuffer[intIndex++] = '0';
    } else {
        while (intTemp > 0) {
            intBuffer[intIndex++] = '0' + (intTemp % 10);
            intTemp /= 10;
        }
    }

    // Reverse and add integer part
    for (int i = intIndex - 1; i >= 0; i--) {
        text[index++] = intBuffer[i];
    }

    // Add decimal point
    text[index++] = '.';

    // Convert fractional part
    char fracBuffer[12];
    int fracIndex = 0;
    if (fractionalDigits == 0) {
        fracBuffer[fracIndex++] = '0';
    } else {
        while (fractionalDigits > 0) {
            fracBuffer[fracIndex++] = '0' + (fractionalDigits % 10);
            fractionalDigits /= 10;
        }
    }

    // Add leading zeros for smaller fractions
    for (int i = fracIndex; i < decimalPlaces; i++) {
        text[index++] = '0';
    }

    // Reverse and add fractional part
    for (int i = fracIndex - 1; i >= 0; i--) {
        text[index++] = fracBuffer[i];
    }

    text[index] = 0x04;

    OutStr(text);
}

SerialLib Serial = {
    .printInt = printInt,
    .printStr = printStr,
    .printlnInt = printlnInt,
    .printlnStr = printlnStr,
		.printlnFloat =printlnFloat,
		.printFloat =printFloat,
};


