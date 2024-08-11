class Nodo:
    def __init__(self, numero, cor):
        self.numero = numero
        self.cor = cor
        self.proximo = None

class ListaEncadeada:
    def __init__(self):
        self.head = None
        self.numero_a = 201
        self.numero_v = 1
        self.atendidos_a = 0
        self.atendidos_v = 0
        self.ultimo_atendido = None

    def inserirSemPrioridade(self, nodo):
        if self.head is None:
            self.head = nodo
        else:
            atual = self.head
            while atual.proximo:
                atual = atual.proximo
            atual.proximo = nodo

    def inserirComPrioridade(self, nodo):
        if self.head is None:
            self.head = nodo
        else:
            atual = self.head
            anterior = None
            while atual and atual.cor == 'A':
                anterior = atual
                atual = atual.proximo
            if anterior is None:
                nodo.proximo = self.head
                self.head = nodo
            else:
                nodo.proximo = anterior.proximo
                anterior.proximo = nodo

    def inserir(self):
        cor = input("Digite a cor do cartão (A/V): ").upper()
        if cor == 'A':
            numero = self.numero_a
            self.numero_a += 1
        elif cor == 'V':
            numero = self.numero_v
            self.numero_v += 1
        else:
            print("Cor inválida. Por favor, insira 'A' ou 'V'.")
            return

        nodo = Nodo(numero, cor)
        if self.head is None:
            self.head = nodo
        elif cor == 'V':
            self.inserirSemPrioridade(nodo)
        elif cor == 'A':
            self.inserirComPrioridade(nodo)
        
        print(f"Paciente registrado com cartão número: {numero} e cor: {self.traduzirCor(cor)}")
        print(" ")

    def traduzirCor(self, cor):
        return "Amarelo" if cor == 'A' else "Verde"

    def imprimirListaEspera(self):
        if self.head is None:
            print("A lista de espera está vazia.")
        else:
            atual = self.head
            amarelos_fila = 0
            verdes_fila = 0
            print("Lista de Espera:")
            print(f"{'Número':<10}{'Cor':<10}")
            print("-" * 40)
            while atual:
                print(f"{atual.numero:<10}{self.traduzirCor(atual.cor):<10}")
                if atual.cor == 'A':
                    amarelos_fila += 1
                elif atual.cor == 'V':
                    verdes_fila += 1
                atual = atual.proximo
            
            print(" ")
            print("Resumo da Fila:")
            print("-" * 40)
            print(f"Pacientes na fila: {amarelos_fila + verdes_fila}")
            print(f"Pacientes com cartão amarelo na fila: {amarelos_fila}")
            print(f"Pacientes com cartão verde na fila: {verdes_fila}")
            print(f"Pacientes com cartão amarelo atendidos: {self.atendidos_a}")
            print(f"Pacientes com cartão verde atendidos: {self.atendidos_v}")
            print("-" * 40)
            print(" ")

            if self.head:
                print(f"Próximo paciente a ser chamado: Cartão {self.head.numero}, Cor {self.traduzirCor(self.head.cor)}")
            
            if self.ultimo_atendido:
                print(f"Atendendo paciente com cartão número: {self.ultimo_atendido.numero}")

    def atenderPaciente(self):
        if self.head is None:
            print("Não há pacientes na fila.")
        else:
            print(f"Chamando paciente com cartão número: {self.head.numero}, Cor {self.traduzirCor(self.head.cor)}")
            if self.head.cor == 'A':
                self.atendidos_a += 1
            else:
                self.atendidos_v += 1
            self.ultimo_atendido = self.head
            self.head = self.head.proximo

    def menu(self):
        while True:
            print("Menu:")
            print("1 – Adicionar paciente à fila")
            print("2 – Mostrar pacientes na fila")
            print("3 – Chamar paciente")
            print("4 – Sair")
            opcao = input("Escolha uma opção: ")
            if opcao == '1':
                self.inserir()
            elif opcao == '2':
                self.imprimirListaEspera()
            elif opcao == '3':
                self.atenderPaciente()
            elif opcao == '4':
                print("Encerrando o programa.")
                break
            else:
                print("Opção inválida, tente novamente.")

def main():
    fila = ListaEncadeada()
    fila.menu()

if __name__ == "__main__":
    main()
