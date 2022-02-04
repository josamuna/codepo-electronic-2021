/* 
 * File:   adc.h
 * Author: mosee
 */

#ifndef ADC_H
#define	ADC_H

/**
 * En-tÕte de la librairie de l'ADC pour ELEC-H309
 * 
 * Ces fonctions utilisent l'ADC1 du dsPIC pour convertir le signal de AN0.
 * 
 * L'ADC peut Õtre utilis» dans 2 modes, par adcInit() :
 *   - en "manuel" : les conversions sont d»marr»es en appelant adcStart(). 
 *     C'est donc la boucle principale qui rythme l'»chantillonnage
 *   - d»clench» par le timer3 : dans ce mode, une conversion est d»marr»e 
 *     ? chaque d»bordement du timer3. La fr»quence d'»chantillonnage est alors
 *     d»finie par PR3.
 * Comme une conversion prend plusieurs cycles d'horloge, il faut utiliser 
 * adcConversionDone() pour "poller" la fin d'une conversion.
 * L'»chantillon peut alors Õtre lu avec adcRead().
 */


#define ADC_MANUAL_SAMPLING 0
#define ADC_TIMER3_SAMPLING 2

/**
 * Initialise l'ADC1 pour convertir AN0, sur 10 bits
 * ParamÀtre:
 *  - mode : d»finit le mode de fonctionnement. Il peut prendre 2 valeurs :
 *     - ADC_MANUAL_SAMPLING : les conversions sont lanc»es par adcStart()
 *     - ADC_TIMER3_SAMPLING : la fr»quence d'»chantillonnage est d»finie
 *          par le TIMER3. NE PAS UTILISER adcStart() DANS CE MODE !
 */
void adcInit(int mode);


/**
 * D»marre une conversion. A n'utiliser qu'en mode ADC_MANUAL_SAMPLING
 */
void adcStart(int pin);

/**
 * renvoie 1 si la conversion est termin»e, 0 sinon
 */
int adcConversionDone(void);

/**
 * Renvoie le r»sultat de la derniÀre conversion.
 * Si on essaie de lire pendant une conversion, la fonction le d»tecte et 
 * renvoie -32768, qui est une valeur impossible comme r»sultat de conversion.
 */
int adcRead(void);


#endif	/* ADC_H */
