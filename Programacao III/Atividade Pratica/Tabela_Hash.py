class Nodo:
    def __init__(self, sigla):
        self.sigla = sigla
        self.proximo = None

class TabelaHash:
    def __init__(self):
        self.tabela = [None] * 10  # 10 posições representando os números de 0 a 9

    def funcao_hash(self, sigla):
        if sigla == "DF":
            return 7  # Regra específica para o Distrito Federal
        # Cálculo da posição usando a função hash
        posicao = (ord(sigla[0]) + ord(sigla[1])) % 10
        return posicao

    def inserir(self, sigla):
        posicao = self.funcao_hash(sigla)
        novo_nodo = Nodo(sigla)
        # Inserir no início da lista encadeada
        if self.tabela[posicao] is None:
            self.tabela[posicao] = novo_nodo
        else:
            novo_nodo.proximo = self.tabela[posicao]
            self.tabela[posicao] = novo_nodo

    def imprimir_tabela(self):
        for i in range(10):
            print(f"Posição {i}: ", end="")
            nodo_atual = self.tabela[i]
            if not nodo_atual:
                print("None", end="")
            while nodo_atual:
                print(f"{nodo_atual.sigla} -> ", end="")
                nodo_atual = nodo_atual.proximo
            print(" ")

def menu():
    tabela_hash = TabelaHash()
    
    while True:
        print("Menu:")
        print("1. Inserir estado")
        print("2. Inserir estado fictício")
        print("3. Imprimir tabela")
        print("4. Sair")
        
        opcao = input("Escolha uma opção: ")
        
        if opcao == "1":
            sigla = input("Digite a sigla do estado: ").upper()
            tabela_hash.inserir(sigla)
            print(f"Estado com sigla {sigla} inserido.")
        
        elif opcao == "2":
            sigla = input("Digite a sigla do estado fictício: ").upper()
            tabela_hash.inserir(sigla)
            print(f"Estado fictício com sigla {sigla} inserido.")
        
        elif opcao == "3":
            tabela_hash.imprimir_tabela()
        
        elif opcao == "4":
            print("Encerrado...")
            break
        
        else:
            print("Opção inválida. Tente novamente.")

# Executa o menu
menu()