/*****************************************************************************\
|                                                                             |
|        _______          __   _________.__                                   |
|        \      \   _____/  |_/   _____/|  |__   ____   ____   ______         |
|        /   |   \_/ __ \   __\_____  \ |  |  \ /  _ \_/ __ \ /  ___/         |  
|       /    |    \  ___/|  | /        \|   Y  (  <_> )  ___/ \___ \          |
|       \____|__  /\___  >__|/_______  /|___|  /\____/ \___  >____  >         |
|               \/     \/            \/      \/            \/     \/          |
|                                                                             |
|    Copyright 2018 (c) Netshoes Todos os direitos reservados.                |
\*****************************************************************************/

"use strict";

import { IGBDialog, GBMinInstance } from "botlib";
import { BotAdapter } from "botbuilder";
import { Messages } from "../strings";

const PasswordGenerator = require("strict-password-generator").default;
const MicrosoftGraph = require("@microsoft/microsoft-graph-client");

export class BuyProductDialog extends IGBDialog {
  token: string;

  /**
   * Setup dialogs flows and define services call.
   *
   * @param bot The bot adapter.
   * @param min The minimal bot instance data.
   */
  static setup(bot: BotAdapter, min: GBMinInstance) {
    min.dialogs.add("/Buy_Product", [
      async dc => {
        let text = ["Masculino ou Feminino?"];

        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = ["Qual o seu peso e altura?"];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = ["Você já corre há algum tempo?"];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = ["Qual seu tipo de pisada?"];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = [
          "Caso queira mais informações sobre o tipo, pode encontrar no http://www.tenis-para-corrida.com/tipos-pisada-importancia-quem-corre"
        ];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = ["Qual a frequência da corrida?"];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;
        let text = ["Quanto você calça?"];
        await dc.prompt("textPrompt", text);
      },
      async (dc, results) => {
        let code = results.response;

        let text = [
          "Essa seria uma ótima opção para o seu estilo de corrida. Para finalizar a compra, basta clicar no link abaixo: https://www.netshoes.com.br/cart"
        ];
        await dc.prompt("textPrompt", text);
      }
    ]);
  }
}
