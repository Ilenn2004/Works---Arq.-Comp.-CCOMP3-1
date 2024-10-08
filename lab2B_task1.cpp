#include <iostream>
#include <bitset>
#include <cmath>
#include <cstdint>
using namespace std;

//Redondeo del significando a 23 bits
bitset<23> RedondearSignificando(bitset<49> significandoCompleto) {
    bitset<23> significandoRedondeado(0);
    for (int i = 0; i < 23; i++) {
        significandoRedondeado[i] = significandoCompleto[i];
    }
    return significandoRedondeado;
}

//Suma binaria de dos bitsets de 49 bits
bitset<49> SumarBitsSignificando(bitset<49> significandoA, bitset<49> significandoB) {
    bitset<49> suma;
    bool acarreo = 0;
    for (int i = 0; i < 49; i++) {
        bool A_bit = significandoA[i];
        bool B_bit = significandoB[i];
        bool sumaBit = A_bit ^ B_bit ^ acarreo;
        acarreo = (A_bit & B_bit) | (A_bit & acarreo) | (B_bit & acarreo);
        suma[i] = sumaBit;
    }
    return suma;
}

//Suma con biased representation en exponentes (8 bits)
bitset<8> SumarExponenteBiased(bitset<8> exponenteA, bitset<8> exponenteB) {
    bitset<8> bias("10000001"); //Bias de 127
    bitset<8> sumaExponente;
    bitset<8> exponenteFinal;
    bool acarreo = 0;
    
    //Sumar los exponentes
    for (int i = 0; i < 8; i++) {
        bool A_bit = exponenteA[i];
        bool B_bit = exponenteB[i];
        bool sumaBit = A_bit ^ B_bit ^ acarreo;
        acarreo = (A_bit & B_bit) | (A_bit & acarreo) | (B_bit & acarreo);
        sumaExponente[i] = sumaBit;
    }

    //Sumar el bias
    acarreo = 0;
    for (int i = 0; i < 8; i++) {
        bool A_bit = sumaExponente[i];
        bool B_bit = bias[i];
        bool sumaBit = A_bit ^ B_bit ^ acarreo;
        acarreo = (A_bit & B_bit) | (A_bit & acarreo) | (B_bit & acarreo);
        exponenteFinal[i] = sumaBit;
    }
    return exponenteFinal;
}

//Normalización del significando, ajusta el exponente
bitset<8> NormalizarSignificando(bitset<49> &significandoCompleto) {
    int bitAlto = 48;
    while (!(significandoCompleto[bitAlto])) {
        bitAlto--;
    }
    
    //Ajustar el significando según la posición del bit más alto
    if (bitAlto - 23 <= 0) {
        significandoCompleto <<= (bitAlto - 23);
    } else {
        significandoCompleto >>= (bitAlto - 23);
    }

    //Ajustar el exponente según la posición del bit más alto
    int nuevoExponente = (bitAlto - 46) + 127;
    bitset<8> exponenteNormalizado(nuevoExponente);
    return exponenteNormalizado;
}

//Multiplicación de los significandos
bitset<49> MultiplicarSignificando(bitset<49> significandoA, bitset<49> significandoB) {
    significandoA[23] = 1; // Añadir el bit implícito
    significandoB[23] = 1; 
    bitset<49> sumaAcumulada(0);
    
    //Multiplicación binaria de los significandos
    for (int i = 0; i < 24; i++) { //23 bits + 1 bit implícito
        if (significandoB[0]) {
            sumaAcumulada = SumarBitsSignificando(significandoA, sumaAcumulada);
        }
        significandoA <<= 1; //Desplazar A a la izquierda
        significandoB >>= 1; //Desplazar B a la derecha
    }
    return sumaAcumulada;
}

//Unión para el float de 32 bits
union Float32Bits {
    float valorFlotante;
    struct {
        uint32_t significando : 23;
        uint32_t exponente : 8;
        uint32_t signo : 1;
    } partes;
};

int main() {
    Float32Bits valor1, valor2;
    cout << "Float 1: ";
    cin >> valor1.valorFlotante;
    cout << "Float 2: ";
    cin >> valor2.valorFlotante;

    bitset<1> signo1(valor1.partes.signo);
    bitset<8> exponente1(valor1.partes.exponente);
    bitset<23> significando1(valor1.partes.significando);

    bitset<1> signo2(valor2.partes.signo);
    bitset<8> exponente2(valor2.partes.exponente);
    bitset<23> significando2(valor2.partes.significando);

    cout << "\nFloat 1: -> Exponente: " << exponente1 << " - Signo: " << signo1 << " - Significando: " << significando1 << " <-" << endl;
    cout << "Float 2: -> Exponente: " << exponente2 << " - Signo: " << signo2 << " - Significando: " << significando2 << " <-\n" <<endl;

    //Si alguno de los valores es 0
    if ((exponente1.none() && significando1.none()) || (exponente2.none() && significando2.none())) {
        cout << "Resultado final - binario: " << bitset<1>(0) << " " << bitset<8>(0) << " " << bitset<23>(0);
		cout << "\nResultado final - float: \"0\"\n";
        return 0;
    }

    bitset<8> exponenteSuma = SumarExponenteBiased(exponente1, exponente2);

    //Verificar overflow y underflow en el exponente
    if ((exponente1[7] == 0 && exponente2[7] == 0) && exponenteSuma[7] == 1) {
        cout << "Float 1 - Underflow de exponente.\n";
        return 0;
    } else if ((exponente1[7] == 1 && exponente2[7] == 1) && exponenteSuma[7] == 0) {
        cout << "Float 1 - Overflow de exponente.\n";
        return 0;
    }

    bitset<49> productoSignificando = MultiplicarSignificando(bitset<49>(valor1.partes.significando), bitset<49>(valor2.partes.significando));
    bitset<8> exponenteNormalizado = NormalizarSignificando(productoSignificando);
    bitset<23> significandoFinal = RedondearSignificando(productoSignificando);
    bitset<8> exponenteFinal = SumarExponenteBiased(exponenteNormalizado, exponenteSuma);

    //Verificar overflow/underflow en el exponente final
    if ((exponenteNormalizado[7] == 0 && exponenteSuma[7] == 0) && exponenteFinal[7] == 1) {
        cout << "Float 2 - Underflow de exponente.\n";
        return 0;
    } else if ((exponenteNormalizado[7] == 1 && exponenteSuma[7] == 1) && exponenteFinal[7] == 0) {
        cout << "Float 2 - Overflow de exponente.\n";
        return 0;
    }
	
	//Verificar overflow/underflow en la representacion
	if (exponenteFinal.to_ulong() < 1) {
    cout << "Underflow de exponente (fuera del rango representable de 8 bits).\n";
	}
	else if (exponenteFinal.to_ulong() > 255) {
    cout << "Overflow de exponente (fuera del rango representable de 8 bits).\n";
	}

    //Determinar el signo final
    bitset<1> signoFinal = signo1 ^ signo2;

    //Mostrar el resultado en binario
    cout << "Resultado final - binario: " << signoFinal << " " << exponenteFinal << " " << significandoFinal << endl;

    //Usando la unión Float32Bits
    Float32Bits resultado;
    resultado.partes.signo = signoFinal.to_ulong();
    resultado.partes.exponente = exponenteFinal.to_ulong();
    resultado.partes.significando = significandoFinal.to_ulong();

    cout << "Resultado final - float: " << resultado.valorFlotante << endl;

    return 0;
}
