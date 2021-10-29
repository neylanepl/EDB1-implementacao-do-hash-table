# Projeto 5 - Implementação do Hash Table
Neste projeto foi implementado a estrutura de dados Tabela de Dispersão em `c++`, que possui comportamento similar ao container associativo `std::unordered_map` do STL.

## Categorias do Projeto
O projeto está organizado da seguinte forma:
```
trabalho-05-hash-table   ---> Nome da pasta do projeto.
├── build                ---> Pasta onde os executáveis serão gerados. [será criada]
├── docs                 ---> Pasta que contém as instruções para a implementação deste trabalho.
├── source               ---> Pasta que contém o código fonte do projeto.
│   ├── driver           ---> Pasta que contém os arquivos para testar o hash table em ação.
│   ├── include          ---> Pasta que contém os arquivos com a implementação do hash table.
│   ├── tests            ---> Pasta que contém os arquivos para os testes unitários do hash table.
│   ├── CMakeLists.txt   ---> Script de configuração do cmake.
├── README.md            ---> Instruções para compilar e executar o programa.
```

## Compilando e Executando
A compilação e execução do projeto pode ser feita usando *cmake*. 

### Como compilar o projeto
1. Instale a biblioteca [Googletest](https://github.com/google/googletest).

2. Entre na pasta principal do projeto.

3. Execute o comando abaixo para criar o diretório de construção e gerar o Makefile baseado no script encontrado em `source/CMakeLists.txt`.
```console
cmake -S source -B build
```

4. Execute o comando abaixo para disparar o processo de compilação dentro da pasta `build`.
Assim, todos os arquivos gerados serão armazenados na pasta `build`.
```console
cmake --build build
```


### Como executar o projeto
 
- Para testar os métodos unitários, execute:
```console
./build/run_tests
```

- E para testar o hash table em ação, execute:
```console
./build/driver_hash
```


## Limitações ou Funcionalidades Não Implementadas no Programa

Analisando as implementações realizadas, acredita-se que todas as funcionalidades esperadas foram desenvolvidas. Além disso, limitações não foram encontradas no projeto.

--------
&copy; Jonas&Neylane/UFRN 2021.