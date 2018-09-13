using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class QuemEhMaisRapidoDialog : IDialog
    {
        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("Eu estou correndo no Azure, posso escalar e ir mais rápido o quanto você precisa. Já o Usain Bolt é feito de carne e osso.");
            context.Done("");
        }
    }
}