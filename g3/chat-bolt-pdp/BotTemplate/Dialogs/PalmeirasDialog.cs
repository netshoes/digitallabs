using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class PalmeirasDialog : IDialog
    {

        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("O Palmeiras? Deixa eu pesquisar...");

            Thread.Sleep(3000);

            await context.PostAsync("Eu procurei em 14 Milhões de paginas diferentes e em nenhuma delas o Palmeiras tem mundial");
            //await context.PostAsync("Deixa eu procurar mais um pouco...");

            Thread.Sleep(1000);

            await context.PostAsync("Quem sabe em outra era!");

            context.Done("");
        }
    }
}