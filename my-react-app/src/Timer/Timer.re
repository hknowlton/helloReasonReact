// define types used by this component
type state = {
  seconds: int,
  isTicking: bool,
};

type action =
  | Start
  | Stop
  | Reset
  | Tick;

// could be seperate, but declaring here
// Just a react functinoal component that takes two props, label and onClick
// Returns a button
module Button = {
  [@react.component]
  let make = (~label, ~onClick) => {
    <button onClick> {label |> ReasonReact.string} </button>;
  };
};

// helper function to update the browser page title.
// the BS raw syntax is telling bucklescript that we are pasting some raw js
let updateTitle: string => unit = [%bs.raw
  {|
  function updateTitle(remaining) {
    document.title = "⏰ " + remaining + " ⏰";
  }|}
];

// main component
[@react.component]
let make = () => {
  // just number formatting helper for timer
  let padNumber = numString =>
    if (numString |> int_of_string < 10) {
      "0" ++ numString;
    } else {
      numString;
    };
  // time formatting helper
  let formatTime = seconds => {
    let mins = seconds / 60;
    let minsString = mins |> string_of_int |> padNumber;
    let seconds = seconds mod 60;
    let secondsString = seconds |> string_of_int |> padNumber;
    minsString ++ ":" ++ secondsString;
  };
  // "stateful" react component with useReducer h00k
  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | Start => {...state, isTicking: true}
        | Stop => {...state, isTicking: false}
        | Reset => {...state, seconds: 30}
        | Tick =>
          state.isTicking && state.seconds > 0
            ? {
              updateTitle(formatTime(state.seconds - 1));
              {...state, seconds: state.seconds - 1};
            }
            : state
        },
      {isTicking: false, seconds: 30},
    );
  // use effect hook, could cleanup here if needed
  React.useEffect0(() => {
    let timerId = Js.Global.setInterval(() => dispatch(Tick), 1000);
    Some(() => Js.Global.clearInterval(timerId));
  });
  // different ways to style but this is easiest
  <div
    style={ReactDOMRe.Style.make(
      ~border="1px solid black",
      ~borderRadius="8px",
      ~maxWidth="580px",
      ~display="flex",
      ~padding="30px",
      ~flexDirection="column",
      (),
    )}>
    // familiar syntax to rendering js with react

      {ReasonReact.string(
         "There are " ++ formatTime(state.seconds) ++ " on the clock",
       )}
      {state.isTicking
         ? <Button label="STOP" onClick={_event => dispatch(Stop)} />
         : <>
             <Button label="START" onClick={_event => dispatch(Start)} />
             <Button label="RESET" onClick={_event => dispatch(Reset)} />
           </>}
    </div>;
};