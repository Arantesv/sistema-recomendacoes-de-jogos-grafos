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
 * Arquivo de implementação (.c) contendo a lógica de todas as funções para o
 * projeto. Inclui a manipulação da estrutura do grafo(inserção, remoção),
 * leitura/gravação de arquivos, conexidade e recomendações de jogos similares.
 *
 * HISTÓRICO DE ALTERAÇÕES:
 * 19/09/2025, Andre Guimaraes: Implementação das funções básicas do grafo (inicializa,
 * libera, criaNo).
 * 20/09/2025, Vitor Arantes e Leonardo Patriani: Implementação da manipulação de arestas/vértices e das
 * rotinas de arquivo (ler, gravar).
 * 21/09/2025, Vitor Arantes: Implementação da análise de conexidade (conexo/desconexo)
 * 22/09/2025, Leonardo Patriani: Implementação da lógica de recomendação
 * 28/09/2025, Andre Guimaraes: Refatoração da função de mostrar conteúdo do arquivo
 *
 *******************************************************************************/

#include "TGrafoLista.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constrói e inicializa as estruturas de um grafo com n vértices.
void inicializaGrafo(Grafo *grafo, int n, int tipo)
{
    grafo->num_vertices = n;
    grafo->num_arestas = 0;
    grafo->tipo = tipo;
    grafo->lista_adj = (No **)malloc(n * sizeof(No *));
    grafo->dados_vertices = (DadosVertice *)malloc(n * sizeof(DadosVertice));

    for (int i = 0; i < grafo->num_vertices; i++)
    {
        grafo->lista_adj[i] = NULL;
        strcpy(grafo->dados_vertices[i].rotulo, "");
    }
}

// Libera toda a memória alocada para o grafo.
void liberaGrafo(Grafo *grafo)
{
    if (!grafo)
        return;
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        No *no_atual = grafo->lista_adj[i];
        while (no_atual)
        {
            No *no_anterior = no_atual;
            no_atual = no_atual->prox;
            free(no_anterior);
        }
    }
    free(grafo->lista_adj);
    free(grafo->dados_vertices);
    grafo->num_vertices = 0;
    grafo->num_arestas = 0;
}

// Cria um novo nó para a lista de adjacências.
No *criaNo(int vertice, int peso)
{
    No *novo_no = (No *)malloc(sizeof(No));
    novo_no->vertice_adj = vertice;
    novo_no->peso = peso;
    novo_no->prox = NULL;
    return novo_no;
}

// Insere uma aresta entre dois vértices, com um determinado peso.
void insereAresta(Grafo *grafo, int v_origem, int v_destino, int peso)
{
    // Insere aresta v_origem -> v_destino
    No *no_atual = grafo->lista_adj[v_origem];
    while (no_atual != NULL)
    {
        if (no_atual->vertice_adj == v_destino)
            return; // Aresta já existe
        no_atual = no_atual->prox;
    }
    No *novo = criaNo(v_destino, peso);
    novo->prox = grafo->lista_adj[v_origem];
    grafo->lista_adj[v_origem] = novo;

    // Insere v_destino -> v_origem - para grafo não direcionado
    if (grafo->tipo < 4)
    {
        novo = criaNo(v_origem, peso);
        novo->prox = grafo->lista_adj[v_destino];
        grafo->lista_adj[v_destino] = novo;
    }
    grafo->num_arestas++;
}

// Remove uma aresta entre dois vértices.
void removeAresta(Grafo *grafo, int v_origem, int v_destino)
{
    // Ponteiros para percorrer a lista de adjacências do vértice de origem.
    // 'no_anterior' é usado para "religar" a lista após a remoção do 'no_atual'.
    No *no_atual = grafo->lista_adj[v_origem];
    No *no_anterior = NULL;

    // Procura na lista de adjacências pelo nó que representa a aresta para v_destino.
    while (no_atual != NULL && no_atual->vertice_adj != v_destino)
    {
        no_anterior = no_atual;
        no_atual = no_atual->prox;
    }
    // Se o nó foi encontrado (no_atual != NULL), procede com a remoção.
    if (no_atual != NULL)
    {
        // O nó a ser removido é o primeiro da lista (cabeça).
        if (no_anterior == NULL)
        {
            // A cabeça da lista de adjacências passa a ser o próximo nó.
            grafo->lista_adj[v_origem] = no_atual->prox;
        }
        // O nó está no meio ou no fim da lista.
        else
        {
            // O nó anterior passa a apontar para o nó seguinte ao atual, "pulando-o".
            no_anterior->prox = no_atual->prox;
        }
        free(no_atual);
        // Decrementa o contador de arestas do grafo.
        grafo->num_arestas--;
    }

    // Remove a aresta no sentido inverso
    if (grafo->tipo < 4)
    {
        // Repete a mesma lógica, mas agora para a lista de adjacências de v_destino,
        // procurando pela aresta que aponta de volta para v_origem.
        no_atual = grafo->lista_adj[v_destino];
        no_anterior = NULL;
        while (no_atual != NULL && no_atual->vertice_adj != v_origem)
        {
            no_anterior = no_atual;
            no_atual = no_atual->prox;
        }
        // Se a aresta de volta for encontrada, remove-a.
        if (no_atual != NULL)
        {
            if (no_anterior == NULL)
                grafo->lista_adj[v_destino] = no_atual->prox;
            else
                no_anterior->prox = no_atual->prox;
            free(no_atual);
        }
    }
}

// Insere um novo vértice no grafo.
void insereVertice(Grafo *grafo, const char *rotulo)
{
    // Armazena o número atual de vértices para usar como índice do novo vértice.
    int n_antigo = grafo->num_vertices;
    // Calcula o novo total de vértices após a adição.
    int n_novo = n_antigo + 1;

    grafo->lista_adj = (No **)realloc(grafo->lista_adj, n_novo * sizeof(No *));
    grafo->dados_vertices = (DadosVertice *)realloc(grafo->dados_vertices, n_novo * sizeof(DadosVertice));

    // A lista de adjacências do novo vértice (no índice n_antigo) começa vazia (NULL).
    grafo->lista_adj[n_antigo] = NULL;
    // Copia o rótulo fornecido para a estrutura de dados do novo vértice.
    strcpy(grafo->dados_vertices[n_antigo].rotulo, rotulo);

    // Atualiza o contador total de vértices no grafo.
    grafo->num_vertices = n_novo;
    printf("Jogo '%s' adicionado com o ID %d.\n", rotulo, n_antigo);
}

// Remove um vértice do grafo e todas as arestas a ele conectadas.
void removeVertice(Grafo *grafo, int v_remover)
{
    if (v_remover < 0 || v_remover >= grafo->num_vertices)
    {
        printf("Erro: Vértice %d é inválido para remoção.\n", v_remover);
        return;
    }

    char rotulo_removido[100];
    strcpy(rotulo_removido, grafo->dados_vertices[v_remover].rotulo);

    // Remove todas as arestas ligadas ao vértice
    No *no_atual = grafo->lista_adj[v_remover];
    while (no_atual != NULL)
    {
        removeAresta(grafo, v_remover, no_atual->vertice_adj);
        no_atual = grafo->lista_adj[v_remover]; // Reavalia o início da lista
    }

    // Move o último vértice para a posição do vértice removido
    int ultimo_v_idx = grafo->num_vertices - 1;
    if (v_remover != ultimo_v_idx)
    {
        // Copia os dados do último vértice
        strcpy(grafo->dados_vertices[v_remover].rotulo, grafo->dados_vertices[ultimo_v_idx].rotulo);

        // Remove arestas que apontavam para o último vértice, antes de mover a lista
        for (int i = 0; i < ultimo_v_idx; i++)
        {
            removeAresta(grafo, i, ultimo_v_idx);
        }

        // Move a lista de adjacências e recria as arestas com o novo ID
        No *no_movido = grafo->lista_adj[ultimo_v_idx];
        grafo->lista_adj[v_remover] = NULL; // Limpa a lista do vértice removido

        while (no_movido != NULL)
        {
            insereAresta(grafo, v_remover, no_movido->vertice_adj, no_movido->peso);
            no_movido = no_movido->prox;
        }
        liberaGrafo((Grafo *)&(grafo->lista_adj[ultimo_v_idx])); // Libera a lista antiga
    }

    // Reduz o tamanho do grafo
    grafo->num_vertices--;
    if (grafo->num_vertices > 0)
    {
        grafo->lista_adj = (No **)realloc(grafo->lista_adj, grafo->num_vertices * sizeof(No *));
        grafo->dados_vertices = (DadosVertice *)realloc(grafo->dados_vertices, grafo->num_vertices * sizeof(DadosVertice));
    }
    else
    {
        free(grafo->lista_adj);
        free(grafo->dados_vertices);
        grafo->lista_adj = NULL;
        grafo->dados_vertices = NULL;
    }

    printf("Jogo '%s' (ID %d) foi removido.\n", rotulo_removido, v_remover);
    if (v_remover != ultimo_v_idx)
    {
        printf("Aviso: O jogo do ID %d foi movido para o ID %d.\n", ultimo_v_idx, v_remover);
    }
}

// Exibe o grafo em formato de lista de adjacências.
void mostraGrafo(Grafo *grafo)
{
    printf("\n--- Mostrando Grafo ---\n");
    printf("Vértices: %d | Arestas: %d | Tipo: %d\n", grafo->num_vertices, grafo->num_arestas, grafo->tipo);
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        printf("[%d] %s:", i, grafo->dados_vertices[i].rotulo);
        No *no_atual = grafo->lista_adj[i];
        while (no_atual)
        {
            printf(" -> %d (Peso:%d)", no_atual->vertice_adj, no_atual->peso);
            no_atual = no_atual->prox;
        }
        printf("\n");
    }
    printf("--- Fim do Grafo ---\n");
}

Grafo *lerArquivo(const char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo para leitura");
        return NULL;
    }

    int tipo, num_v, num_a_entradas;
    fscanf(arquivo, "%d", &tipo);
    fscanf(arquivo, "%d", &num_v);

    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    inicializaGrafo(grafo, num_v, tipo);

    char buffer_rotulo[100], buffer_peso[10];
    for (int i = 0; i < num_v; i++)
    {
        int id;
        fscanf(arquivo, "%d \"%[^\"]\" \"%[^\"]\"", &id, buffer_rotulo, buffer_peso);
        strcpy(grafo->dados_vertices[id].rotulo, buffer_rotulo);
    }

    fscanf(arquivo, "%d", &num_a_entradas);

    for (int i = 0; i < num_a_entradas; i++)
    {
        int u, v, w;
        fscanf(arquivo, "%d %d %d", &u, &v, &w);
        insereAresta(grafo, u, v, w);
    }

    fclose(arquivo);
    return grafo;
}

void gravarArquivo(const char *nome_arquivo, Grafo *grafo)
{
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo para gravação");
        return;
    }

    fprintf(arquivo, "%d\n", grafo->tipo);
    fprintf(arquivo, "%d\n", grafo->num_vertices);
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        fprintf(arquivo, "%d \"%s\" \"0\"\n", i, grafo->dados_vertices[i].rotulo);
    }

    int num_entradas_arestas = (grafo->tipo < 4) ? grafo->num_arestas * 2 : grafo->num_arestas;
    fprintf(arquivo, "%d\n", num_entradas_arestas);

    for (int i = 0; i < grafo->num_vertices; i++)
    {
        No *no_atual = grafo->lista_adj[i];
        while (no_atual)
        {
            if (grafo->tipo >= 4 || i < no_atual->vertice_adj)
            {
                fprintf(arquivo, "%d %d %d\n", i, no_atual->vertice_adj, no_atual->peso);
                if (grafo->tipo < 4)
                {
                    fprintf(arquivo, "%d %d %d\n", no_atual->vertice_adj, i, no_atual->peso);
                }
            }
            no_atual = no_atual->prox;
        }
    }
    fclose(arquivo);
    printf("Grafo salvo com sucesso em '%s'.\n", nome_arquivo);
}

void mostraConteudoArquivo(const char *nome_arquivo)
{
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir o arquivo para leitura");
        return;
    }

    printf("\n--- Arquivo: '%s' ---\n", nome_arquivo);

    int tipo_grafo;
    fscanf(arquivo, "%d", &tipo_grafo);
    printf("\n[ TIPO DO GRAFO ]\n");
    switch (tipo_grafo)
    {
    case 0:
        printf("  Tipo: 0 - Grafo não orientado sem peso\n");
        break;
    case 1:
        printf("  Tipo: 1 - Grafo não orientado com peso no vértice\n");
        break;
    case 2:
        printf("  Tipo: 2 - Grafo não orientado com peso na aresta\n");
        break;
    case 3:
        printf("  Tipo: 3 - Grafo não orientado com peso nos vértices e arestas\n");
        break;
    case 4:
        printf("  Tipo: 4 - Grafo orientado sem peso\n");
        break;
    case 5:
        printf("  Tipo: 5 - Grafo orientado com peso no vértice\n");
        break;
    case 6:
        printf("  Tipo: 6 - Grafo orientado com peso na aresta\n");
        break;
    case 7:
        printf("  Tipo: 7 - Grafo orientado com peso nos vértices e arestas\n");
        break;
    default:
        printf("  Tipo: Desconhecido\n");
        break;
    }

    int num_vertices;
    fscanf(arquivo, "%d", &num_vertices);
    printf("\n[ VÉRTICES (%d) ]\n", num_vertices);
    printf("+------+---------------------------------------------------+\n");
    printf("|  ID  | Rótulo (Nome do Jogo)                             |\n");
    printf("+------+---------------------------------------------------+\n");

    char buffer_rotulo[100];
    char buffer_peso[20];
    for (int i = 0; i < num_vertices; i++)
    {
        int id;
        if (fscanf(arquivo, "%d \"%[^\"]\" \"%[^\"]\"", &id, buffer_rotulo, buffer_peso) == 3)
        {
            printf("| %-4d | %-49s |\n", id, buffer_rotulo);
        }
    }
    printf("+------+---------------------------------------------------+\n");

    int num_arestas;
    fscanf(arquivo, "%d", &num_arestas);
    printf("\n[ ARESTAS (%d entradas no arquivo) ]\n", num_arestas);
    printf("+--------+---------+------+\n");
    printf("| Origem | Destino | Peso |\n");
    printf("+--------+---------+------+\n");

    for (int i = 0; i < num_arestas; i++)
    {
        int u, v, w;
        if (fscanf(arquivo, "%d %d %d", &u, &v, &w) == 3)
        {
            printf("|   %-4d |   %-5d | %-4d |\n", u, v, w);
        }
        else
        {

            break;
        }
    }
    printf("+--------+---------+------+\n");

    printf("\n--- Fim do Conteúdo do Arquivo ---\n");
    fclose(arquivo);
}

// Busca um vértice pelo seu nome (rótulo) e retorna o ID.
int buscaVerticePorNome(Grafo *grafo, const char *nome)
{
    // Se o ponteiro do grafo for nulo, retorna erro.
    if (!grafo)
        return -1;

    // Percorre todos os vértices do grafo, do ID 0 até o último.
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        // Usa a função strcmp para comparar o nome procurado com o rótulo do vértice atual.
        // strcmp retorna 0 se as duas strings forem exatamente iguais.
        if (strcmp(grafo->dados_vertices[i].rotulo, nome) == 0)
        {
            // Se encontrou uma correspondência, retorna imediatamente o ID (índice) do vértice.
            return i;
        }
    }
    return -1;
}

// Obtém um ID de vértice a partir da entrada do usuário (pode ser ID ou nome).
int obtemIdPorEntrada(Grafo *grafo)
{
    char entrada[100];
    fgets(entrada, sizeof(entrada), stdin);
    entrada[strcspn(entrada, "\n")] = 0;

    if (entrada[0] == '\0')
        return -1;

    // Tenta converter a string de entrada para um número.
    // A função strtol converte o que for número e nos diz onde parou através do ponteiro 'end_ptr'.
    char *end_ptr;
    long id_potencial = strtol(entrada, &end_ptr, 10);

    // Avança o ponteiro 'end_ptr' para ignorar quaisquer espaços em branco
    // que o usuário possa ter digitado após o número (ex: "12  ").
    while (isspace((unsigned char)*end_ptr))
        end_ptr++;

    // Sse 'end_ptr' aponta para o final da string ('\0'),
    // significa que TODOS os caracteres da entrada foram convertidos em número.
    if (*end_ptr == '\0')
    {
        // Valida se o ID existe no grafo.
        if (id_potencial >= 0 && id_potencial < grafo->num_vertices)
        {
            return (int)id_potencial;
        }
        else
        {
            return -1;
        }
    }
    // Se 'end_ptr' não aponta para o final, a string continha caracteres não numéricos.
    else
    {
        return buscaVerticePorNome(grafo, entrada);
    }
}

// Estrutura auxiliar para ordenação das recomendações
typedef struct Recomendacao
{
    int id;
    int peso;
} Recomendacao;

// Função de comparação para o qsort.
int comparaRecomendacoes(const void *a, const void *b)
{
    return ((Recomendacao *)b)->peso - ((Recomendacao *)a)->peso;
}

// Exibe uma lista de jogos similares ao jogo informado.
void obtemRecomendacoes(Grafo *grafo, int id_vertice)
{
    printf("\nJogos similares a '%s':\n", grafo->dados_vertices[id_vertice].rotulo);

    // Acessa a lista de adjacências do vértice informado.
    // Esta lista contém todos os vizinhos diretos (jogos similares).
    No *no_atual = grafo->lista_adj[id_vertice];

    // Verifica se a lista de adjacências está vazia.
    if (!no_atual)
    {
        printf("Nenhum jogo similar encontrado.\n");
        return; // Encerra a função se não houver recomendações.
    }

    // É necessário percorrer a lista uma vez para saber quantos itens existem
    // e alocar a memória necessária para ordená-los.
    int contador = 0;
    No *temporario = no_atual;
    while (temporario)
    {
        contador++;
        temporario = temporario->prox;
    }

    // Aloca um array de Recomendacao do tamanho exato do número de vizinhos.
    Recomendacao *recomendacoes = (Recomendacao *)malloc(contador * sizeof(Recomendacao));

    // Percorre a lista de adjacências novamente, desta vez copiando os dados.
    int i = 0;
    while (no_atual)
    {
        // Copia o ID do jogo vizinho e o peso da aresta (similaridade) para o array.
        recomendacoes[i].id = no_atual->vertice_adj;
        recomendacoes[i].peso = no_atual->peso;
        i++;
        no_atual = no_atual->prox;
    }

    // Usa a função para ordenar o array.
    // A função 'comparaRecomendacoes' é usada para ordenar em ordem decrescente de peso.
    qsort(recomendacoes, contador, sizeof(Recomendacao), comparaRecomendacoes);

    printf("---------------------------------------------\n");
    printf("| %-30s | Similaridade |\n", "Jogo Recomendado");
    printf("---------------------------------------------\n");

    // Percorre o array agora ordenado e imprime cada recomendação.
    for (i = 0; i < contador; i++)
    {
        printf("| %-30s | %-12d |\n", grafo->dados_vertices[recomendacoes[i].id].rotulo, recomendacoes[i].peso);
    }
    printf("---------------------------------------------\n");
    free(recomendacoes);
}

// Função auxiliar que realiza a busca em profundidade (DFS) para a verificação de conexão.
void buscaEmProfundidadeAuxiliar(Grafo *grafo, int v_atual, int visitado[])
{
    visitado[v_atual] = 1;
    No *no_vizinho = grafo->lista_adj[v_atual];
    while (no_vizinho)
    {
        if (!visitado[no_vizinho->vertice_adj])
        {
            buscaEmProfundidadeAuxiliar(grafo, no_vizinho->vertice_adj, visitado);
        }
        no_vizinho = no_vizinho->prox;
    }
}

// Verifica se um grafo não direcionado é conexo.
// Retorna 1 se for conexo, e 0 se for desconexo.
int verificaConexao(Grafo *grafo)
{
    if (grafo->num_vertices == 0)
    {
        return 1; // Um grafo vazio é considerado conexo.
    }

    int *visitado = (int *)calloc(grafo->num_vertices, sizeof(int));

    // Inicia a busca a partir do primeiro vértice (ID 0).
    buscaEmProfundidadeAuxiliar(grafo, 0, visitado);

    // Verifica se todos os vértices foram visitados.
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        // Se um vértice não foi visitado, o grafo não é conexo.
        if (visitado[i] == 0)
        {
            free(visitado);
            return 0;
        }
    }

    free(visitado);
    return 1; // Conexo
}

// Função principal que apresenta o resultado da análise de conexidade.
void apresentaConexidade(Grafo *grafo)
{
    printf("\n--- Análise de Conexidade para Grafo Não Direcionado ---\n");
    if (grafo->num_vertices == 0)
    {
        printf("O grafo está vazio, mas é considerado conexo.\n");
        return;
    }

    if (verificaConexao(grafo))
    {
        printf("Resultado: O grafo é CONEXO.\n");
    }
    else
    {
        printf("Resultado: O grafo é DESCONEXO.\n");
    }
}

typedef struct InfoGrau
{
    int id;
    int grau;
} InfoGrau;

int comparaGraus(const void *a, const void *b)
{
    return ((InfoGrau *)b)->grau - ((InfoGrau *)a)->grau; // Decrescente
}

// 1. ANÁLISE DE GRAUS (Popularidade/Conectividade)
void apresentaGraus(Grafo *grafo)
{
    if (grafo->num_vertices == 0)
    {
        printf("Grafo vazio.\n");
        return;
    }

    InfoGrau *lista_graus = (InfoGrau *)malloc(grafo->num_vertices * sizeof(InfoGrau));

    for (int i = 0; i < grafo->num_vertices; i++)
    {
        int contador = 0;
        No *atual = grafo->lista_adj[i];
        while (atual)
        {
            contador++;
            atual = atual->prox;
        }
        lista_graus[i].id = i;
        lista_graus[i].grau = contador;
    }

    // Ordena para mostrar os "mais conectados" primeiro
    qsort(lista_graus, grafo->num_vertices, sizeof(InfoGrau), comparaGraus);

    printf("\n--- Grau dos Vértices (Conectividade) ---\n");
    printf("| %-4s | %-40s | %-4s |\n", "ID", "Jogo", "Grau");
    printf("|------|------------------------------------------|------|\n");

    // Mostra top 10 ou todos se tiver menos que 10
    int limite = grafo->num_vertices; // Pode mudar para 10 se quiser resumir
    for (int i = 0; i < limite; i++)
    {
        int id_real = lista_graus[i].id;
        printf("| %-4d | %-40.40s | %-4d |\n",
               id_real,
               grafo->dados_vertices[id_real].rotulo,
               lista_graus[i].grau);
    }
    printf("--------------------------------------------------\n");
    free(lista_graus);
}

void verificaEuleriano(Grafo *grafo)
{
    printf("\n--- Análise Euleriana ---\n");

    if (!verificaConexao(grafo))
    {
        printf("Resultado: O grafo NÃO é Euleriano (é desconexo).\n");
        printf("Explicação: Para ser Euleriano, o grafo precisa ser conexo.\n");
        return;
    }

    // Conta vértices de grau ímpar
    int impares = 0;
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        int grau = 0;
        No *atual = grafo->lista_adj[i];
        while (atual)
        {
            grau++;
            atual = atual->prox;
        }
        if (grau % 2 != 0)
        {
            impares++;
        }
    }

    printf("Número de vértices com grau ímpar: %d\n", impares);

    if (impares == 0)
    {
        printf("Resultado: O grafo é EULERIANO.\n");
        printf("Existe um ciclo que percorre todas as similaridades e volta ao jogo inicial.\n");
    }
    else if (impares == 2)
    {
        printf("Resultado: O grafo é SEMI-EULERIANO.\n");
        printf("Existe um caminho que percorre todas as similaridades, mas começa e termina em jogos diferentes.\n");
    }
    else
    {
        printf("Resultado: O grafo NÃO é Euleriano.\n");
        printf("Possui mais de 2 vértices de grau ímpar.\n");
    }
}

// Retorna o grau de um vértice específico
int obterGrau(Grafo *grafo, int v)
{
    int grau = 0;
    No *atual = grafo->lista_adj[v];
    while (atual)
    {
        grau++;
        atual = atual->prox;
    }
    return grau;
}

// Verifica se dois vértices são adjacentes (vizinhos)
int saoAdjacentes(Grafo *grafo, int u, int v)
{
    No *atual = grafo->lista_adj[u];
    while (atual)
    {
        if (atual->vertice_adj == v)
            return 1;
        atual = atual->prox;
    }
    return 0;
}

int verificarTeoremasHamiltonianos(Grafo *grafo)
{
    int n = grafo->num_vertices;
    if (n < 3)
    {
        printf("Grafo muito pequeno para ser Hamiltoniano (N < 3).\n");
        return 0;
    }

    printf("\n--- Análise de Teoremas Hamiltonianos ---\n");

    // 1. Teorema de Dirac
    // Se grau(v) >= n/2 para TODO v, então é Hamiltoniano.
    int satisfazDirac = 1;
    for (int i = 0; i < n; i++)
    {
        if (obterGrau(grafo, i) < n / 2.0)
        {
            satisfazDirac = 0;
            break;
        }
    }

    if (satisfazDirac)
    {
        printf("[Teorema de Dirac]: ATENDIDO.\n");
        printf("O grafo é Hamiltoniano.\n");
        printf("Todos os vértices têm grau >= %d (metade de N).\n", n / 2);
        return 1;
    }
    else
    {
        printf("[Teorema de Dirac]: Não atendido (existem vértices com grau baixo).\n");
    }

    // 2. Teorema de Ore
    // Se para todo par não-adjacente (u,v), grau(u) + grau(v) >= n, então é Hamiltoniano.
    int satisfazOre = 1;
    for (int u = 0; u < n; u++)
    {
        for (int v = u + 1; v < n; v++)
        {
            if (!saoAdjacentes(grafo, u, v))
            {
                if (obterGrau(grafo, u) + obterGrau(grafo, v) < n)
                {
                    satisfazOre = 0;
                    goto FimOre;
                }
            }
        }
    }

FimOre:
    if (satisfazOre)
    {
        printf("[Teorema de Ore]: ATENDIDO.\n");
        printf("O grafo é Hamiltoniano.\n");
        return 1;
    }
    else
    {
        printf("[Teorema de Ore]: Não atendido (soma de graus de não-vizinhos é baixa).\n");
    }
    printf("O grafo ainda pode ser Hamiltoniano, mas não garantido pelos teoremas clássicos.\n");
    return 0;
}