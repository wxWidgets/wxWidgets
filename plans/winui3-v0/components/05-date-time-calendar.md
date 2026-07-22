# Fiche 05 — DatePicker, TimePicker et Calendar

Baseline `20207bdbbb`. Fichiers : `src/winui/datectrl.cpp`, `timectrl.cpp`,
`calctrl.cpp`. Dépend des adapters enabled/tooltip/UIA/DPI.

## wxDatePickerCtrl — alpha

Deux échecs sont observés : borne de date et focus. `SetValue()` conserve une
valeur hors plage et ignore l’échec peer (`datectrl.cpp:123-130,158-175`).
Supprimer une borne ne restaure pas le défaut natif (`:137-150,166-170`).

**Plan :**

1. faire du modèle value/min/max l’autorité ;
2. `SetRange()` clamp la valeur existante et rétablit les bornes natives quand
   une borne devient invalide/absente ;
3. `SetValue()` hors range suit exactement le contrat des autres ports ;
4. `ALLOWNONE` round-trip sans événement parasite ;
5. caractériser `SHOWCENTURY`, `SPIN`/`DROPDOWN` et fallback explicite ;
6. corriger preferred focus target et conversions date/timezone.

**Tests :** `datepickerctrltest.cpp` vert, allow-none, add/remove bounds, DST,
focus, programmatic no-event, keyboard, disable, locale/DPI.

## wxTimePickerCtrl — beta

WinUI n’affiche pas les secondes mais l’adapter les écrit/lit
(`timectrl.cpp:136-167`); une édition peut les perdre. Les valeurs invalides sont
silencieusement ignorées (`:119-124`).

**Décision :** soit composer un champ secondes, soit documenter que la V0
normalise à la minute. Conserver silencieusement puis perdre les secondes n’est
pas acceptable.

**Tests :** secondes non nulles, 12/24 h et locales, invalid value, mutation
user/programmatic, Enter/focus, disabled, hot theme et DPI.

## wxCalendarCtrl — beta, alpha si vitrine publique

### Manques confirmés

- `EnableMonthChange()` et `Mark()` no-op (`calctrl.cpp:160-170`) ;
- seuls selection/double-click sont câblés (`:93-111,207-223`) ;
- DoubleTapped sur toute CalendarView émet la date courante sans hit-test ;
- retirer une borne laisse la valeur précédente active (`:141-198`) ;
- events page/day/month/year/week et styles navigation/holidays/colours manquent.

### Plan

1. corriger modèle/bornes comme DatePicker ;
2. hit-tester/identifier un day item avant DOUBLECLICKED ;
3. mapper DisplayDateChanged et selection aux events de navigation wx avec ordre
   et veto documentés ;
4. implémenter `NO_MONTH_CHANGE`/EnableMonthChange ;
5. mapper Mark et attributs de jour via CalendarViewDayItemChanging ;
6. traiter jours fériés, premier jour, couleurs, locales, UIA.

### Tests

Jour vs header/nav double-click, tous événements, veto, month disabled, marks,
holidays, add/remove bounds, allow none si applicable, locale/DST, keyboard,
Narrator, 100/150/200 % et hot theme.

## Sortie

Alpha : DatePicker vert et Calendar nominal sans faux événement/crash si exposé.
Beta : toutes API de navigation/mark annoncées, secondes TimePicker décidées,
UIA/locales/DPI/theme validés.

