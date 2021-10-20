// Contador binário representando distancia do sonar e acendedor de leds em duas ordens(crescente e decrescente)

// Desenvolvido por Marcos, João Pedro, José Américo e Vinicius Piacini

int qtdLeds = 5;  // quantidade total de leds
int leds[5] = { 11,10,9,6,5 }; // lista com os pinos dos leds 

// constantes auxiliares
const float distMin = 2.0;
const float distMax = 32.0;
const byte interruptPin = 2;

// guarda o valor do maior e de menor indice no array
const int maiorIndice = qtdLeds-1;
const int menorIndice = 0;
const int bits = qtdLeds; // sera utilizada na conversão de decimal para binário
const int bitMaior = 1<<(bits-1); // maior valor possível de um bit
                                  // em relação a sua posição 

float ret_eco;  // variavel que guarda o valor decimal retornado
			    // do dispositivo sonico
volatile float dist = 0.0; // distancia entre sonar e ponto
int led=bitMaior; 		// led atual que estará aceso no modo em que acende e apaga os leds em ordem
int cresce = 0; 		// Controla ordem de leds que acendem ou apagam
int atual=bitMaior;		//guarda o valor decimal que informa quais leds
						// dever ser acessos ou apagados
int anterior=0;	// guarda o conjunto de leds que foi acesso ou apago anteriormente

//pinos para o sensor sonar TRIG e ECHO
int eco = 2;
int trig = 3;

void setup()
{
  	// Inicializa os pinos dos leds indo de 0 até menor que a 
 	// qtdLeds pois estes são todos os indices válidos do
    // array leds
    for(int i = 0 ; i<qtdLeds; i++)
    {
      	pinMode(leds[i],OUTPUT);
    }
  
  	// inicializa os pinos do dispositivo sonico 
  	pinMode(trig,OUTPUT);
  	pinMode(eco,INPUT);
  
  	// iniciliaza o pino do eco para disparar interrupção na
  	// subida do sinal
    attachInterrupt(digitalPinToInterrupt(eco), calcDist, FALLING);    
}

void loop()
{
	// Função sonar que vai detectar a distancia do objeto
  	// e disparar uma interrupção
	sonar();
  
  	// lida com a variavel que será mostrada através dos leds
  
	if(distMin < dist && dist < distMax ){
		atual = int(dist);
    }
       
    if (dist >= distMax) 
    {				 
             
      	atual = led;
        
      	// caso deva crescer( ou seja o valor de led ser um valor maior )
        if (cresce==1)
        	led<<=1; // multiplica por 2 para fazer o efeito de crescer
      				 // exemplo:
      				 // 0001 -> cresce para: 0010
      				 //           ou seja:
      				 //    1 -> cresce para:   2
        else
            led>>=1; // analogo porém com divisão, causando visualmente o 
      				 // efeito inverso
		// nesse trecho if else verfica se esta em algum extremo,
      	// se estiver então inverte a ordem de crescimento ou decrescimento
        if (led==bitMaior)
            cresce=0;
        else if (led==1)
            cresce=1;
    }
    
  	// função que mostra os bits corretos conforme o valor decimal informado
	dec_para_bin( anterior, atual );
  	anterior = atual;  // preserva o valor atual
}

// Representa número em 
// binário através dos leds
// Desenvolvemos uma abordagem que utiliza
// uma mascara de bits em que se preocupa
// em configurar ou resetar ( acender ou 
// apagar no nosso caso) um bit ( led ) por vez
// nesta versão da função incorporamos também para
// que receba o valor anterior e faça uma comparação
// e somente apague ou acenda as posições necessárias

void dec_para_bin(int anterior, int destino)
{	
  	// i será usado para percorrer todos os indices
  	// válidos do array leds
  	// como j será nossa mascara de bits, ele deve passar
  	// por todas posições válidas, imagine nosso número decimal
  	// convertido para binário ocupando no máximo 4 bits
  	// então o j deverá 1000 , 0100 , 0010 e 0001 que são 
  	// 8, 4, 2 e 1, iremos percorrer cada vez indo do bit
  	// mais significativo até o menor, então o maior j será
  	// 2 ^ 3 , por isso podemos definir anteriormente o bitMaior
  	// como sendo 1<<(bits-1)
    int i,
  		bitPosicional=bitMaior, 
  		bitDoDestino, bitDoAnterior;
  
    for (i=maiorIndice; i>=menorIndice; i--)
    {
    	bitDoDestino = destino & bitPosicional;
      	bitDoAnterior = anterior & bitPosicional;
		
      	// Nesse primeiro teste verifica se o número que vai ser apresentado
      	// agora tem o seu bit atual modificado em relação ao bit correspondente
      	// do numero anteriormente representado
      	// ex.: 
      	//  numero anterior 18 -> 10100
      	//	numero destino  13 -> 01101
      	//					        ||
     	// 							^^
      	// veja que esses bits não precisão ser modificados portanto aplicando
      	// um xor do bit que quero acender ou apagar com o numero anterior então
      	// eu sei a partir disso se devo ou não modificalo
      	// destino 		 			13 -> 01101
      	// bitPosicional 			16 -> 10000
      	//					   		  	  -----
      	// destino & bitPosicional	   -> 00000
      
      	// faz o mesmo com o 
      	// anterior e então 
      	// faz o bitwise xor
      	// entre eles:
      	// anterior & bitPosicional	18 -> 10000
      	// 					   			  -----
      	// bitDoDestino ^ bitDoAnterior-> 10000 , número diferente de zero então quer dizer
      	//								que modificou
        if ( bitDoDestino ^ bitDoAnterior ) {
          
            // Confere se no número a ser apresentado
            // o bit atual esta como 0 ou 1, ignorando o
            // o estado dos outros:
            // ex.: destino =   7  0111
            //		bitPosi =   2  0010
            //			  i =   1
            // logo vai acender o led 1
            if (bitDoDestino)
            {
                acende(i);
            }
            else
            {
                apaga(i);
            }
        }
      	// a cada passo divide bitPosicional por 2 pois começamos com
      	// ele testando o bit mais significativo ( ou o led
      	// mais a esquerda )
		bitPosicional>>=1;
    }
}

// Funções auxiliares para acender e 
// apagar o led no indice indicado
void acende(int i)
{
	analogWrite(leds[i] , 255 );
	delay(42);
}

void apaga(int i)
{
	analogWrite(leds[i] , 0 );
	delay(42);
}

// Capta a distancia através do sonar 
// cada vez que o eco do sonar estiver
// em high vai disparar uma interrupção
void sonar()
{
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  ret_eco = pulseIn(eco,HIGH);
}

// função no estilo Interrupt Service Routine 
// que será chamada quando houver uma interrupção
// causada pelo pino de eco
void calcDist() {
  dist = ((ret_eco/2) * 0.034); 
}
