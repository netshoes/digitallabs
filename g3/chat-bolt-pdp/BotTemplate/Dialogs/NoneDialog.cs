using System;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class NoneDialog : IDialog
    {
        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("Desulpe, não entendi.");
            context.Done("");
        }
    }
}