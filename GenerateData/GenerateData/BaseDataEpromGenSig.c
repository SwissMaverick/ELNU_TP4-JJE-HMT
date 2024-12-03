//-----------------------------------------------------------------------------------//
// Nom du projet 		: Generation Code EPROM
// Nom du fichier 		: BaseDataEpromGenSig.c
// Date de création 	: 28.11.2016
// Date de modification : 29.11.2018
//
// Auteur 				: C. HUBER 
//						  Philou (Ph. Bovey)
//
// Description          : le programme génère un fichier ".hex"
//
//
// Remarques :            lien pour la table ASCII :
// 						  -> http://www.asciitable.com/
// 						  lien pour la saisie de clavier avec getc & getchar pour ne pas
//                        avoir des erreurs d'interprétation
// 						  -> http://fr.openclassrooms.com/informatique/cours/utiliser-les-bonnes-fonctions-d-entree
//----------------------------------------------------------------------------------//
// Programmation EPROM pour le générateur de signal 
// SL122_ELNU TP SLO1 2016-2017
// fichier de base
// C. HUBER 28.11.2016

#include <stdio.h>
#include <math.h>

//  _CRT_SECURE_NO_WARNINGS

// Nom du fichier à générer 		**** A MODIFIER le nom de fichier ****
char *FileName = "TestGR1.hex";

#define NB_MAX_ECH 240

// Table des échantillons
// Cette table est à remplir avec votre algorithme par la fonction
// GenerateData
unsigned char TableEchantillons[NB_MAX_ECH];


// Nom du groupe 				**** A MODIFIER la chaîne de caractère ****
char *GroupeName = "Nom1-Nom2";
//  A adapter en commentant l'un ou l'autre
const unsigned short MemStart = 0 ;						// mécanisme de reset
// const unsigned short MemStart = 256 - NB_MAX_ECH ;		// mécanisme de recharge


//**** FONCTION A MODIFIER ****
// GenerateData
// Cette fonction rempli la table des 96 valeurs pour l'Eprom 
// Le résulat est placé dans TableEchantillons
void GenerateData (void)
{
	int i, T, t, f;

	T = 480;
	t = 0;
	i = 0;

	f = 1 / T;
	
	
	for (i = 1; i <= NB_MAX_ECH; i++) 
	{
		
		TableEchantillons[i] = i;	// pour contrôle
	}

} // GenerateData


int main (void)
{
	unsigned char UserAnswer;

	FILE *fp1;	// descripteur de fichier 
	unsigned short DataAddr, CheckSum, i;
	unsigned short NoEch;
	unsigned short NbMaxEch, MaxDataAddr, MaxTrancheEch ;
	unsigned char  MsbAddr, LsbAddr, ValData, ValFill;
	

	printf("Groupe %s SLO1 2016-2017 \n", GroupeName);
	if (MemStart == 0) {
		printf("Mecanisme de reset, MemStart = %d \n", MemStart);
	} else {
		printf("Mecanisme de recharge , MemStart = %d \n", MemStart);
	}

	printf("Nombre echantillons %d en hexa %X \n", NB_MAX_ECH, NB_MAX_ECH);

	printf("Generation du fichier pour la programmation O/N ! \n");
	scanf_s ("%c%*c", &UserAnswer, 2);

	if ( UserAnswer == 'O' || UserAnswer == 'o' ) {

		DataAddr = 0;	// adresse des données dans le fichier
		// Ouverture du fichier pour écriture
		fp1 = fopen (FileName, "w");

		// Generation des données
		GenerateData();

		if (MemStart != 0)   {
						
			// Remplissage avec FF jusqu'au 1er échantillon
			// par tranche de 16
			for ( ; DataAddr < 256; DataAddr +=16) {

				// Ecriture des données dans le fichier (Intel Hex)
				fprintf(fp1, ":10%04X00",DataAddr);
		
				// Initialisation du CheckSum
				CheckSum = 0x10;	// correspond à NN
				MsbAddr = DataAddr >> 8;
				LsbAddr = (unsigned char)DataAddr;
				CheckSum += MsbAddr;
				CheckSum += LsbAddr;

				ValFill = 0xFF;
				printf ("Addr: %02X  ", DataAddr);

				// Ligne de  de donnée
				for (i = DataAddr ; i < (DataAddr + 16) ; i++) {
					
					if (i == MemStart) NoEch = 0;
					if (i >= MemStart) {
						ValData = TableEchantillons[NoEch];
						fprintf(fp1, "%02X",ValData);
						printf ("%02X ", ValData);		// Affichage de controle
						CheckSum += ValData;
						NoEch++;
					} else {
						fprintf(fp1, "%02X",ValFill);
						printf ("%02X ", ValFill);		// Affichage de controle
						CheckSum += ValFill;
					}
				}
				// Calcul et Ecriture du CheckSum
				CheckSum = CheckSum & 0x00FF;
				if (CheckSum != 0)	CheckSum = 256 - CheckSum;
				fprintf(fp1, "%02X\n",CheckSum);
				printf ("\n");

			} // end for
		}  else { 
				// cas MemStart == 0
				NoEch = 0;
				NbMaxEch = NB_MAX_ECH;
				MaxTrancheEch = 16;
				if ((NbMaxEch % 16)  == 0 ) {
					MaxDataAddr = NbMaxEch;	// doit être Multiple de 16
				} else {
					// doit être Multiple de 16
					MaxDataAddr = 16 * ( (NbMaxEch / 16 ) + 1) ;
				}
				ValFill = 0xFF;
				// Boucle  écriture dans le fichier de programmation
				for (DataAddr = 0 ; DataAddr < MaxDataAddr ; DataAddr += 16) {

					// Ecriture des données dans le fichier (Intel Hex)
					fprintf(fp1, ":10%04X00",DataAddr);
		
					// Initialisation du CheckSum
					CheckSum = 0x10;	// correspond à NN
					MsbAddr = DataAddr >> 8;
					LsbAddr = (unsigned char)DataAddr;
					CheckSum += MsbAddr;
					CheckSum += LsbAddr;
					printf ("Addr: %02X  ", DataAddr);

					// Ecriture d'une tranche de donnée
					for ( ; NoEch < MaxTrancheEch ; NoEch++) {

						if (NoEch >= NbMaxEch ) {
							fprintf(fp1, "%02X", ValFill);
							printf ("%02X ", ValFill);		// Affichage de controle
							CheckSum += ValFill;
						} else {
							ValData = TableEchantillons[NoEch];
							fprintf(fp1, "%02X", ValData);
							printf ("%02X ", ValData);		// Affichage de controle
							CheckSum += ValData;
						}
					}
					// calcul et Ecriture du CheckSum
					CheckSum = CheckSum & 0x00FF;
					if (CheckSum != 0)	CheckSum = 256 - CheckSum;
					fprintf(fp1, "%02X\n",CheckSum);
					printf ("\n");
					MaxTrancheEch += 16;	// tranche suivante
				}
		}

		// Remplissage avec FF jusqu'a la fin de l'EEPROM
		// Eprom 8K*8
		for ( ; DataAddr < 8096; DataAddr +=16) {

			// Ecriture des données dans le fichier (Intel Hex)
			fprintf(fp1, ":10%04X00",DataAddr);
		
			// Initialisation du CheckSum
			CheckSum = 0x10;	// correspond à NN
			MsbAddr = DataAddr >> 8;
			LsbAddr = (unsigned char)DataAddr;
			CheckSum += MsbAddr;
			CheckSum += LsbAddr;

			ValData = 0xFF;
			// Ligne de  de donnée
			for (i = 0 ; i < 16 ; i++) {

				fprintf(fp1, "%02X",ValData);
				CheckSum += ValData;
			}
			// calcul et Ecriture du CheckSum
			CheckSum = CheckSum & 0x00FF;
			if (CheckSum != 0)	CheckSum = 256 - CheckSum;
			fprintf(fp1, "%02X\n",CheckSum);

		}
		// Ajout dernière ligne au fichier 
		fprintf(fp1, ":00000001FF\n");

		fclose (fp1);
		printf ("Fichier %s genere acec succes ! \n", FileName);
		printf ("Pressez ENTER  pour terminer ! \n");
		scanf_s ("%*c", &UserAnswer, 1);
	}
	else {
		printf ("Pas de generation du Fichier ! \n");
	}
	return (0);
}