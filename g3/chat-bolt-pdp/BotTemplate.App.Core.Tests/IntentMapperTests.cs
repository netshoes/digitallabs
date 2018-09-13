using NUnit.Framework;
using System.Linq;

namespace BotTemplate.App.Core.Tests
{
    [TestFixture]
    public class IntentMapperTests
    {
        [Test]
        public void Insure_OnlyOneMappingByIntent()
        {
            Assert.IsTrue(
                DIManager.IntentMappers
                .Select(x => x.SupportedIntenties)
                .Distinct().Count() == DIManager.IntentMappers.Count());

        }
    }
}
