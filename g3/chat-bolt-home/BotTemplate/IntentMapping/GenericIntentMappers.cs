using System;
using BotTemplate.Dialogs;
using BotTemplate.Core.Dialogs;
using System.Collections.Generic;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;
using BotTemplate.Integration;
using Autofac;

namespace bottemplate.intentmapping
{
    [Serializable]
    public class GenericIntentMappers : IntentMapper
    {
        public override Dictionary<string, InitialIntent> SupportedIntenties => new Dictionary<string, InitialIntent> {
            { "Greetings", InitGreetingsDialog },
            { "None",  InitNoneDialog },
            { "MeFaleEsporte",  InitFaleEsporte },
            { "ComoFazerUmaTroca",  InitComoFazerUmaTroca },
            { "QuemEhMaisRapidoBolt", InitQuemEhMaisRapidoBolt },
            { "MeLeveParaCorrida", InitDialogIrParaSessaoCorrida },
            { "DicasEsporte", InitDicasEsporte }
        };

        private IDialog InitDicasEsporte(NaturalLanguageResult result)
        {
            return new DicasEsporteDialog(result.Entities);
        }

        private IDialog InitQuemEhMaisRapidoBolt(NaturalLanguageResult result)
        {
            return new QuemEhMaisRapidoDialog();
        }

        private IDialog InitDialogIrParaSessaoCorrida(NaturalLanguageResult result)
        {
            return new PerguntarSeDesejaEntrarNaSessaoCorrida();
        }

        private IDialog InitGreetingsDialog(NaturalLanguageResult result)
        {
            return new GreetingsDialog();
        }

        private IDialog InitNoneDialog(NaturalLanguageResult result)
        {
            return new NoneDialog();
        }

        private IDialog InitFaleEsporte(NaturalLanguageResult result)
        {
            return new FaleEsporteDialog(result.Entities);
        }

        private IDialog InitComoFazerUmaTroca(NaturalLanguageResult result)
        {
            return new ComoFazerUmaTroca();
        }
    }
}