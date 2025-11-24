import requests
import time

CLIENT_ID = 'njthnpihhrm07ldoxcm4who6ez8j0y'
CLIENT_SECRET = 'uf3non2edbxwf6g1n62dqp64zsaa7j'
NOME_ARQUIVO = 'grafo.txt'
QTD_JOGOS = 60

def obter_token():
    print("Autenticando na IGDB/Twitch...")
    url = 'https://id.twitch.tv/oauth2/token'
    payload = {
        'client_id': CLIENT_ID,
        'client_secret': CLIENT_SECRET,
        'grant_type': 'client_credentials'
    }
    response = requests.post(url, data=payload)
    if response.status_code != 200:
        raise Exception(f"Erro na autenticação: {response.text}")
    return response.json()['access_token']

def buscar_jogos_populares(token):
    print(f"Baixando lista dos {QTD_JOGOS} jogos mais populares...")
    url = "https://api.igdb.com/v4/games"
    headers = {
        'Client-ID': CLIENT_ID,
        'Authorization': f'Bearer {token}',
        'Accept': 'application/json'
    }

    body = f"""
        fields name, total_rating_count, first_release_date,
               genres, themes, 
               franchises, 
               collections,
               game_modes, 
               player_perspectives,
               involved_companies.company, involved_companies.developer;
        sort total_rating_count desc;
        limit {QTD_JOGOS};
        where themes != null & genres != null;
    """
    
    response = requests.post(url, headers=headers, data=body)
    if response.status_code != 200:
        raise Exception(f"Erro ao buscar jogos: {response.text}")
    
    return response.json()

def extrair_ids_desenvolvedoras(jogo):
    ids = set()
    if 'involved_companies' in jogo:
        for item in jogo['involved_companies']:
            if item.get('developer') is True:
                comp = item.get('company')
                if isinstance(comp, dict):
                    ids.add(comp['id'])
                else:
                    ids.add(comp)
    return ids

def calcular_peso(jogo_a, jogo_b):
    franquias_a = set(jogo_a.get('franchises', []))
    franquias_b = set(jogo_b.get('franchises', []))
    
    colecoes_a = set(jogo_a.get('collections', []))
    colecoes_b = set(jogo_b.get('collections', []))
    
    # Se compartilham Franquia o é peso 10
    if not franquias_a.isdisjoint(franquias_b) or not colecoes_a.isdisjoint(colecoes_b):
        return 10

    devs_a = extrair_ids_desenvolvedoras(jogo_a)
    devs_b = extrair_ids_desenvolvedoras(jogo_b)
    
    generos_a = set(jogo_a.get('genres', []))
    generos_b = set(jogo_b.get('genres', []))
    
    temas_a = set(jogo_a.get('themes', []))
    temas_b = set(jogo_b.get('themes', []))
    
    modos_a = set(jogo_a.get('game_modes', []))
    modos_b = set(jogo_b.get('game_modes', []))
    
    persp_a = set(jogo_a.get('player_perspectives', []))
    persp_b = set(jogo_b.get('player_perspectives', []))

    pontos = 0
    
    # Gênero: +2 pontos
    pontos += len(generos_a.intersection(generos_b)) * 2    
       
    # Tema: +1 ponto
    pontos += len(temas_a.intersection(temas_b)) * 1
    
    # Modos de jogo: +1 ponto fixo se houver alguma correspondência
    if not modos_a.isdisjoint(modos_b):
        pontos += 1
    
    # Perspectiva: +1 ponto fixo se houver alguma correspondência
    if not persp_a.isdisjoint(persp_b):
        pontos += 1
    
    # Mesma desenvolvedora = peso minimo 6
    mesma_dev = not devs_a.isdisjoint(devs_b)
    
    if mesma_dev:
        pontos = max(pontos, 6)
    
    # Limita o peso máximo a 10
    return min(10, pontos)

def gerar_arquivo_grafo(jogos):
    print("Calculando similaridades (Franquias + Coleções)...")
    arestas = []
    
    for i in range(len(jogos)):
        for j in range(i + 1, len(jogos)):
            peso = calcular_peso(jogos[i], jogos[j])
            
            if peso >= 4:
                arestas.append(f"{i} {j} {peso}")

    print(f"Total de arestas criadas: {len(arestas)}")

    with open(NOME_ARQUIVO, 'w', encoding='utf-8') as f:
        f.write("2\n")
        f.write(f"{len(jogos)}\n")
        
        for i, jogo in enumerate(jogos):
            nome_limpo = jogo['name'].replace('"', '').replace('\n', '')
            f.write(f'{i} "{nome_limpo}" "0"\n')
        
        f.write(f"{len(arestas)}\n")
        for aresta in arestas:
            f.write(f"{aresta}\n")
            
    print(f"\nSucesso! Arquivo '{NOME_ARQUIVO}' gerado.")

try:
    token = obter_token()
    lista_jogos = buscar_jogos_populares(token)
    gerar_arquivo_grafo(lista_jogos)
except Exception as e:
    print(f"Ocorreu um erro: {e}")