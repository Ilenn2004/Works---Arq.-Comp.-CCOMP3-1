/******************************************************************************
Algoritmo de multiplicación de enteros con signo.
CCOMP 3-1 - Saravia Apaza, Ilenne Damaris
*******************************************************************************/
#include <iostream>
#include <bitset>
using namespace std;

//funciones utlizadas
bitset<16> a_Binario(int entero, int n);
bitset<16> a_2compliment(bitset<16> int_bit, int n);
bitset<16> adicion_binaria(bitset<16> A_valor, bitset<16> B_valor, int n);
void shift_derecha(bitset<16>& A_valor, bitset<16>& Q_valor, bool& Q_1_valor, int n);
void multiplicacion_booth(int multiplicando, int multiplicador, int n);

bitset<16> a_Binario(int entero, int n) {
    return bitset<16>(entero & ((1 << n) - 1));
}

bitset<16> a_2compliment(bitset<16> int_bit, int n) {
    bitset<16> complemento = ~int_bit;
    bitset<16> uno = bitset<16>(1);
    return adicion_binaria(complemento, uno, n);
}

bitset<16> adicion_binaria(bitset<16> A_valor, bitset<16> B_valor, int n) {
    bitset<16> resultado;
    bool acarreo = 0;
    for (int i = 0; i < n; i++) {
        resultado[i] = A_valor[i] ^ B_valor[i] ^ acarreo;
        acarreo = (A_valor[i] & B_valor[i]) | (B_valor[i] & acarreo) | (A_valor[i] & acarreo);
    }
    return resultado;
}

void shift_derecha(bitset<16>& A_valor, bitset<16>& Q_valor, bool& Q_1_valor, int n) {
    bool MSB_A_valor = A_valor[n - 1];
    Q_1_valor = Q_valor[0];
    Q_valor >>= 1;
    Q_valor[n - 1] = A_valor[0];
    A_valor >>= 1;
    A_valor[n - 1] = MSB_A_valor;
}

void multiplicacion_booth(int multiplicando, int multiplicador, int n) {
    bitset<16> M_valor = a_Binario(multiplicando, n);
    bitset<16> Q_valor = a_Binario(multiplicador, n);
    bitset<16> A_valor = 0;
    bool Q_1_valor = 0;
    int contador = n;

    while (contador > 0) {
        if (Q_valor[0] == 1 && Q_1_valor == 0) {
            A_valor = adicion_binaria(A_valor, a_2compliment(M_valor, n), n);
        } else if (Q_valor[0] == 0 && Q_1_valor == 1) {
            A_valor = adicion_binaria(A_valor, M_valor, n);
        }
        shift_derecha(A_valor, Q_valor, Q_1_valor, n);
        contador--;
    }

    int resultado = (A_valor.to_ulong() << n) | (Q_valor.to_ulong());
    
    if (resultado >= (1 << n)) {
        resultado -= (1 << (n + 1));
    }

    cout << "Valor final de A: " << A_valor << endl;
    cout << "Valor final de Q: " << Q_valor << endl;
    cout << "Resultado: " << resultado << " (" << bitset<16>(resultado) << " en binario)" << endl;
}

int main() {
    int multiplicando, multiplicador, n;

    cout << "Multiplicando: ";
    cin >> multiplicando;
    cout << "Multiplicador: ";
    cin >> multiplicador;
    cout << "n: ";
    cin >> n;

    multiplicacion_booth(multiplicando, multiplicador, n);

    return 0;
}
