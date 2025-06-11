# 📨 Tópicos MQTT Utilizados no Projeto (1 Tanque)

Este documento descreve todos os tópicos MQTT utilizados no projeto de monitoramento de nível de água de um único tanque, utilizando ESP32, sensor ultrassônico HC-SR04, válvula, bomba e display LCD I2C.

---

## Prefixo de tópicos

O prefixo `tanque/` foi escolhido para representar o contexto deste projeto simples de **um tanque**.

---

## 1. `tanque/distancia`

* **Tipo:** Publicador
* **Publicado por:** ESP32
* **Formato da mensagem:** Número inteiro em centímetros (ex: `45`)
* **Descrição:**
  Distância medida pelo sensor ultrassônico HC-SR04.

---

## 2. `tanque/porcentagem`

* **Tipo:** Publicador
* **Publicado por:** ESP32
* **Formato da mensagem:** Número decimal com duas casas (ex: `75.32`)
* **Unidade:** Percentual (%)
* **Descrição:**
  Percentual de enchimento do tanque, calculado a partir da distância medida.

---

## 3. `tanque/valvula/status`

* **Tipo:** Publicador
* **Publicado por:** ESP32
* **Formato da mensagem:** `ON` ou `OFF`
* **Descrição:**
  Estado da válvula de controle de água. Abre (`ON`) quando o nível está abaixo do limite inferior (20%), e fecha (`OFF`) quando acima do limite superior (80%).

---

## 4. `tanque/bomba/status`

* **Tipo:** Publicador
* **Publicado por:** ESP32
* **Formato da mensagem:** `ON` ou `OFF`
* **Descrição:**
  Estado da bomba de água. Liga (`ON`) sempre que a válvula estiver aberta, e desliga (`OFF`) quando a válvula estiver fechada.

---

## 5. `tanque/config/threshold`

* **Tipo:** Assinante
* **Assinado por:** ESP32
* **Formato da mensagem:** Número decimal em centímetros (ex: `16.0`)
* **Descrição:**
  Permite atualizar em tempo real o limite de distância (sensor-à-água) abaixo do qual o tanque é considerado cheio (equivalente a 80% de enchimento). Afeta o controle da válvula imediatamente.

---

## Observações Gerais

* **Broker:** `test.mosquitto.org`
* **Porta:** `1883`
* **Transporte:** TCP/IP
* **QoS:** 0
* **Formato de mensagens:** UTF-8