/*******************************************************************************
 *
 * AUTORES:
 * Andre Moreira Guimaraes - RA: 10416590
 * Leonardo Patriani Cardoso - RA: 10417188
 * Vitor Arantes Vendramini - RA: 10417759
 *
 * TURMA: 06G
 *
 * DESCRIÇÃO:
 * Arquivo de cabeçalho (.h) para o projeto.
 * Define as estruturas de dados principais (Grafo, No, DadosVertice) e os
 * protótipos de todas as funções utilizadas para manipular o grafo e a
 * aplicação.
 *
 * HISTÓRICO DE ALTERAÇÕES:
 * 19/09/2025, Andre Guimaraes: Criação da estrutura inicial do grafo com base no código
 * do professor e adaptação para o problema de recomendação.
 * 20/09/2025, Vitor Arantes e Leonardo Patriani: Adição das funções de manipulação de vértices/arestas
 * e das rotinas de leitura/escrita de arquivo.
 * 21/09/2025, Vitor Arantes: Implementação da análise de conexidade (conexo/desconexo)
 * 28/09/2025, Andre Guimaraes: Limpeza final do código
 *
 *******************************************************************************/
#ifndef T_GRAFO_LISTA_H
#define T_GRAFO_LISTA_H

#include <stdio.h>

// Estrutura para um nó da lista de adjacências
typedef struct No
{
    int vertice_adj; // Vértice de destino (adjacente)
    int peso;        // Peso da aresta (similaridade)
    struct No *prox;
} No;

// Estrutura para os dados de um vértice (Nome do Jogo)
typedef struct DadosVertice
{
    char rotulo[100];
} DadosVertice;

// Estrutura principal do Grafo
typedef struct Grafo
{
    int num_vertices;             // Número de vértices (n)
    int num_arestas;              // Número de arestas (m)
    int tipo;                     // Tipo do grafo (conforme o .pdf)
    No **lista_adj;               // Vetor de listas de adjacências
    DadosVertice *dados_vertices; // Vetor com os dados (rótulos) dos vértices
} Grafo;

void inicializaGrafo(Grafo *grafo, int n, int tipo);
void liberaGrafo(Grafo *grafo);
No *criaNo(int vertice, int peso);
void insereAresta(Grafo *grafo, int v_origem, int v_destino, int peso);
void removeAresta(Grafo *grafo, int v_origem, int v_destino);
void insereVertice(Grafo *grafo, const char *rotulo);
void removeVertice(Grafo *grafo, int v_remover);
void mostraGrafo(Grafo *grafo);
Grafo *lerArquivo(const char *nome_arquivo);
void gravarArquivo(const char *nome_arquivo, Grafo *grafo);
void mostraConteudoArquivo(const char *nome_arquivo);
int obtemIdPorEntrada(Grafo *grafo);
int buscaVerticePorNome(Grafo *grafo, const char *nome);
void obtemRecomendacoes(Grafo *grafo, int id_vertice);
void apresentaConexidade(Grafo *grafo);
void apresentaGraus(Grafo *grafo);
void verificaEuleriano(Grafo *grafo);
int verificarTeoremasHamiltonianos(Grafo *grafo);

#endif